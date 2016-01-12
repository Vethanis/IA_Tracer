#include "myglheaders.h"
#include "ival.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "glscreen.h"
#include "zpyramid.h"
#include "glm/gtx/compatibility.hpp"
#include "glmprint.h"
#include "frustum.h"
#include "omp.h"

using namespace std;
using namespace glm;

#define NEAR 0.1f
#define FAR 20.0f

inline ival map(const ival3& p){
	return isphere4(p, {0.0f, 0.0f, 0.0f}, 1.0f);
}

inline ival trace(const Frustum& f, ival t){
	return map(f.getInterval(t));
}

ival* trace(const Frustum& f, ival t, float e){
	for(int i = 0; i < 60; i++){
		ival2 ts = split(t);
		ival d = trace(f, ts.x);
		//cout << "F(B): "; print(d);
		if(d.contains(0.0f)){
			t = ts.x;
			if(t.width() < e)
				return new ival(t);
			continue;
		}
		d = trace(f, ts.y);
		//cout << "F(B): "; print(d);
		if(d.contains(0.0f)){
			t = ts.y;
			if(t.width()  < e)
				return new ival(t);
			continue;
		}
		return nullptr;
	}
	return nullptr;
}

void split(std::vector<ival3>& s, const ival3& uvt){
	ival2 u = split(uvt.x);
	ival2 v = split(uvt.y);
	s.push_back(ival3(u.x, v.x, uvt.z));
	s.push_back(ival3(u.x, v.y, uvt.z));
	s.push_back(ival3(u.y, v.x, uvt.z));
	s.push_back(ival3(u.y, v.y, uvt.z));
}

struct SubArgs{
	ival3 uvt;
	float e;
	int depth;
	SubArgs(const ival3& a, float b, int c)
		: uvt(a), e(b), depth(c){};
};

// dont use odd thread count
void getStartingUVs(unsigned threads, std::vector<glm::vec4>& uvs){
	unsigned divx = 0;
	unsigned divy = 0;
	unsigned p = 1;
	while(p < threads){
		if(divy < divx){
			divy++;
			p = p << 1;
		}
		else {
			divx++;
			p = p << 1;
		}
	}
	unsigned xres = 1 << divx;
	unsigned yres = 1 << divy;
	const float dx = 2.0f / xres;
	const float dy = 2.0f / yres;
	cout << xres << " " << yres << " : " << dx << " " << dy << endl;
	for(unsigned i = 0; i < p; i++){
		unsigned r = yres*i / p;
		unsigned c = i % xres;
		cout << c << " " << r << endl;
		float xmin = -1.0f + c * dx;
		float xmax = -1.0f + dx + c * dx;
		float ymin = -1.0f + r * dy;
		float ymax = -1.0f + dy + r * dy;
		uvs.push_back({xmin, xmax, ymin, ymax});
	}
}

void subdivide(const Camera& cam, ZPyramid& pyr, int i, float e){
	ival2 uv((i & 1) ? vec2( 0.0f, 1.0f) : vec2(-1.0f, 0.0f),
			(i & 2) ? vec2(-1.0f, 0.0f) : vec2( 0.0f, 1.0f));
	int depth = 1;
	const int max_depth = 4;//pyr.getMaxDepth();
	vector<SubArgs> stack;
	stack.push_back(iavec3(uv.xy(), uv.zw(), {cam.getNear(), pyr(depth, {0.0f, 0.0f})} ));
	while(depth <= max_depth && !stack.empty()){
		//cout << "DEPTH : " << depth << endl;
		mat3 uvt = stack.back();
		stack.pop_back();
		//cout << "UVT: \n"; print(uvt);
		uvt[2] = vec3(trace(cam.getPoints(uvt), e) * (cam.getFar() - cam.getNear()), 0.0f);
		if(uvt[2].y < pyr(depth, icenter(uvt[0].xy(), uvt[1].xy())) ){
			pyr.paint(depth, uvt);
			e = e * 0.5f;
			depth++;
			if(depth <= max_depth)
				split(stack, uvt);
		}
	}
}

inline float distSquared(const glm::vec3& a, const glm::vec3& b){
	return glm::dot(b - a, b - a);
}

int main(int argc, char* argv[]){
	if(argc < 3){
		cout << argv[0] << " <screen width> <screen height>" << endl;
		return 1;
	}
	
	std::vector<vec4> a;
	getStartingUVs(32, a);

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
	pyramid.clear(FAR);
	const int pf = pyramid.getMaxDepth();
	
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WH.x, WH.y);
	camera.update();
    
	vec3 ambient(0.001f, 0.0005f, 0.0005f);
    vec3 light_pos(2.f, 2.f, 2.f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
	//subdivide(camera, pyramid, 0, 1.0f);
	//return 0;
	
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
			
		/*
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
		*/

		#pragma omp parallel for num_threads(4) schedule(static, 1)
		for(int k = 0; k < 4; k++){
			subdivide(camera, pyramid, k, 0.1f);
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

