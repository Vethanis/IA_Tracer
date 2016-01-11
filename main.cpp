#include "myglheaders.h"
#include "interval.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "glscreen.h"
#include "zpyramid.h"
#include "glm/gtx/compatibility.hpp"
#include "omp.h"

using namespace std;
using namespace glm;

#define NEAR 0.1f
#define FAR 10.0f

inline vec2 map(const mat3& p){
	return isphere4(p, {0.0f, 0.0f, 0.0f}, 1.0f);
}

inline float distSquared(const glm::vec3& a, const glm::vec3& b){
	return glm::dot(b - a, b - a);
}

inline vec2 trace(const vec3& ro, const vec3& rd, vec2 t){
	return map(iadd(ro, imul(rd, t)));
}

float trace(const vec3& ro, const vec3& rd, float e){
	vec2 t = vec2(NEAR, FAR);
	if(!icontains(trace(ro, rd, t), 0.0f)) return -1.0f;
	for(int i = 1; i < 60; i++){
		float th = icenter(t);
		vec2 t0 = vec2(t.x, th);
		vec2 d = trace(ro, rd, t0);
		if(icontains(d, 0.0f)){
			t.y = th;
			if(std::max(abs(d.x), abs(d.y)) < e)
				return icenter(t);
			continue;
		}
		t0 = vec2(th, t.y);
		d = trace(ro, rd, t0);
		if(icontains(d, 0.0f)){
			t.x = th;
			if(std::max(abs(d.x), abs(d.y)) < e)
				return icenter(t);
			continue;
		}
		float dd = (t.y - t.x);
		t.x += dd;
		t.y += dd*2.0f;
		if(t.x >= FAR) return -1.0f;
	}
	return -1.0f;
}

vec2 trace(const mat4& uvt, const vec2& t){
	return map(iavec3(lerp(uvt[0].xyz(), uvt[1].xyz(), t.x), lerp(uvt[2].xyz(), uvt[3].xyz(), t.y)));
}

vec2 trace(const mat4& uvt, float e){
	vec2 t = vec2(0.0f, 1.0f);
	for(int i = 1; i < 60; i++){
		float th = icenter(t);
		vec2 t0 = vec2(t.x, th);
		vec2 d = trace(uvt, t0);
		if(icontains(d, 0.0f)){
			t.y = th;
			if(std::max(abs(d.x), abs(d.y)) < e)
				return t;
			continue;
		}
		t0 = vec2(th, t.y);
		d = trace(uvt, t0);
		if(icontains(d, 0.0f)){
			t.x = th;
			if(std::max(abs(d.x), abs(d.y)) < e)
				return t;
			continue;
		}
		float dd = (t.y - t.x);
		t.x += dd;
		t.y += dd*2.0f;
		if(t.x >= FAR) return vec2(0.0f, 1.0f);
	}
	return vec2(0.0f, 1.0f);
}

void split(std::vector<glm::mat3>& s, const glm::mat3& m){
	const vec2 min = m[0].xy();
	const vec2 max = m[1].xy();
	const vec2 t   = m[2].xy();
	const vec2 c   = icenter(min, max);
	s.push_back(iavec3({min.x, c.y}, {c.x, max.y}, t));
	s.push_back(iavec3(c, max, t));
	s.push_back(iavec3(min, c, t));
	s.push_back(iavec3({c.x, min.y}, {max.x, c.y}, t));
}

void subDivide(const Camera& cam, ZPyramid& pyr, float e){
	int depth = 0;
	const int max_depth = pyr.getMaxDepth();
	vector<mat3> stack;
	stack.push_back(iavec3({-1.0f, 1.0f}, {-1.0f, 1.0f}, {cam.getNear(), pyr(depth, {0.0f, 0.0f})} ));
	while(depth <= max_depth && !stack.empty()){
		vec2 t = trace(cam.getPoints(stack.back()), e);
		auto uvt = stack.back();
		stack.pop_back();
		float tmax = pyr(depth, icenter(uvt[0].xy(), uvt[1].xy()));
		if(t.y < tmax){
			uvt[2].xy() = t;
			pyr.paint(depth, uvt);
			depth++;
			split(stack, uvt);
			e = e * 0.5f;
		}
		else
			return;
	}
}

int main(int argc, char* argv[]){
	if(argc < 3){
		cout << argv[0] << " <screen width> <screen height>" << endl;
		return 1;
	}
	const ivec2 WH = ivec2(atoi(argv[1]), atoi(argv[2]));
	const vec2 dwh = vec2(2.0f / WH.x, 2.0f / WH.y);
    const vec2 ddx = vec2(dwh.x, 0.0f);
    const vec2 ddy = vec2(0.0f, dwh.y);
	Window window(WH.x, WH.y, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	prog.bind();
	GLScreen screen(WH.x, WH.y);
	ZPyramid pyramid(WH.x, WH.y);
	const int pf = pyramid.getMaxDepth();
	
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WH.x, WH.y);
	camera.update();
    
	vec3 ambient(0.001f, 0.0005f, 0.0005f);
    vec3 light_pos(2.f, 2.f, 2.f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
	input.poll();
    unsigned i = 0;
    float t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        float dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
		const vec3 ro = camera.getEye();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			light_pos = ro;
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WH.x*WH.y; k++){
			const int c = k % WH.x;
			const int r = k / WH.x;
			const vec2 xy = vec2(c*dwh.x-1.0f, r*dwh.y-1.0f);
			vec3 rd = camera.getRay(xy);
			float t = trace(ro, rd, 0.001f);
			if(t == -1.0f) continue;
			pyramid(pf, xy) = t;
		}
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WH.x*WH.y; k++){
			const int c = clamp(k % WH.x, 1, WH.x-2);
			const int r = clamp(k / WH.x, 1, WH.y-2);
			const vec2 xy = vec2(c*dwh.x-1.0f, r*dwh.y-1.0f);
			const float z = pyramid(pf, xy);
			if(z == FAR) continue;
			vec3 pos = camera.getPoint(xy, z);
			vec3 xv = camera.getPoint(xy+ddx, pyramid(pf, xy+ddx)) - pos;
			vec3 yv = camera.getPoint(xy+ddy, pyramid(pf, xy+ddy)) - pos;
			vec3 N = normalize(cross(xv, yv));
			vec3 L = normalize(light_pos - pos);
			vec3 H = normalize(L + normalize(ro - pos));
			const float D = std::max(0.0f, dot(N, L));
			const float S = pow(std::max(0.0f, dot(H, N)), 16.0f);
			vec3 color = ambient + (D * base_color + S * light_color * base_color) / distSquared(light_pos, pos);
			color = pow(color, vec3(1.0f/2.2f));
			screen.setPixel(k, color);
		}
		
		screen.draw();
        glfwSwapBuffers(window.getWindow());    	
        glClear(GL_COLOR_BUFFER_BIT);
    	screen.clear();
    	pyramid.clear(FAR);
    	
        i++;
        if(i >= 60){
            i = 0;
            printf("ms: %f\n", t / 60.0);
            t = 0.0;
            glfwSetTime(0.0);
        }
    }
    return 0;
}

