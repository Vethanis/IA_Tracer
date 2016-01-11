#include "myglheaders.h"
#include "interval.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "glscreen.h"
#include "zpyramid.h"
#include "omp.h"

using namespace std;
using namespace glm;

#define NEAR 0.1f
#define FAR 10.0f

//void subDivide(camera& cam, 

inline int clamp(int a, int l, int h){
	return std::max(std::min(a, h), l);
}

inline float distSquared(const glm::vec3& a, const glm::vec3& b){
	return glm::dot(b - a, b - a);
}

inline vec2 map(const mat3& p){
	return isphere16(p, {0.0f, 0.0f, 0.0f}, 1.0f);
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

int main(int argc, char* argv[]){
	if(argc < 3){
		cout << argv[0] << " <screen width> <screen height>" << endl;
		return 1;
	}
	int WIDTH = atoi(argv[1]);
	int HEIGHT = atoi(argv[2]);
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	prog.bind();
	GLScreen screen(WIDTH, HEIGHT);
	ZPyramid pyramid(WIDTH, HEIGHT);
	
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WIDTH, HEIGHT);
	camera.update();
    
    const float dx = 2.0f / (float)WIDTH;
	const float dy = 2.0f / (float)HEIGHT;
	const int pf = pyramid.getMaxDepth();
	
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
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			const int r = k / WIDTH;
			const int c = k % WIDTH;
			const float x = c * dx - 1.0f;
			const float y = r * dy - 1.0f;
			vec3 rd = camera.getRay(x, y);
			float t = trace(ro, rd, 0.001f);
			if(t == -1.0f) continue;
			pyramid(pf, x, y) = (float)t;
		}
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			const int r = clamp(k / WIDTH, 1, HEIGHT-2);
			const int c = clamp(k % WIDTH, 1, WIDTH -2);
			const float x = c * dx - 1.0f;
			const float y = r * dy - 1.0f;
			const float z = pyramid(pf, x, y);
			if(z == FAR) continue;
			vec3 pos = camera.getPoint(x, y, z);
			vec3 ddx = camera.getPoint(x+dx, y, pyramid(pf, x+dx, y)) - pos;
			vec3 ddy = camera.getPoint(x, y+dy, pyramid(pf, x, y+dy)) - pos;
			vec3 N = normalize(cross(ddx, ddy));
			vec3 L = normalize(light_pos - pos);
			vec3 H = normalize(L + normalize(ro - pos));
			const float D = std::max(0.0f, dot(N, L));
			const float S = pow(std::max(0.0f, dot(H, N)), 32.0f);
			float dist = distSquared(light_pos, pos);
			vec3 color = ambient + (D * base_color + S * light_color * base_color) / dist;
			color = pow(color, vec3(1.0f/2.2f));
			screen.setPixel(r*WIDTH + c, color);
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

