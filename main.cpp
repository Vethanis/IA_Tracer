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

//void subDivide(camera& cam, 

int clamp(int a, int l, int h){
	return std::max(std::min(a, h), l);
}

float distSquared(const glm::vec3& a, const glm::vec3& b){
	return glm::dot(b - a, b - a);
}

vec2 map(const mat3& p){
	return isphere4(p, {0.0f, 0.0f, 0.0f}, 1.0f);
}

vec2 trace(const vec3& ro, const vec3& rd, vec2 t){
	return map(iadd(ro, imul(rd, t)));
}

float trace(const vec3& ro, const vec3& rd, float e){
	const float zmax = 100.0f;
	vec2 t = vec2(0.1f, zmax);
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
		if(t.x >= zmax) return -1.0f;
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
	mat4 IP = inverse(camera.getP());
	
    
    const float dx = 2.0f / (float)WIDTH;
	const float dy = 2.0f / (float)HEIGHT;
	const float dz = (dx+dy)*0.5f / camera.getFar();
	const int pf = pyramid.getMaxDepth();
	
	vec3 ambient(0.001f, 0.0005f, 0.0005f);
    vec3 light_pos(0.f, 0.f, 2.f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        double dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
		const vec3 ro = camera.getEye();
		const vec4 lpos = camera.getV() * vec4(light_pos, 1.0f);
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			const int r = k / WIDTH;
			const int c = k % WIDTH;
			const float x = c * dx - 1.0f;
			const float y = r * dy - 1.0f;
			vec3 rd = camera.getRay(x, y);
			float t = trace(ro, rd, 0.001f);
			if(t == -1.0f) continue;
			pyramid(pf, x, y) = (double)t / (double)camera.getFar();
		}
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			const int r = clamp(k / WIDTH, 1, HEIGHT-2);
			const int c = clamp(k % WIDTH, 1, WIDTH-2);
			const double x = c * dx - 1.0f;
			const double y = r * dy - 1.0f;
			if(pyramid(pf, x, y) == 1.0f) continue;
			vec4 pos = IP * vec4(x, y, pyramid(pf, x, y), 1.0);
			pos = pos / pos.w;
			vec3 N = normalize(vec3(pyramid(pf, x+dx, y) - pyramid(pf, x-dx, y),
									pyramid(pf, x, y+dy) - pyramid(pf, x, y-dy),
									dz / pyramid(pf, x, y)));
			vec3 L = normalize(vec3( 0.0f, 0.0f, 1.0f));
			vec3 H = normalize(L + vec3(0.0f, 0.0f, 1.0f));
			const float D = std::max(0.0f, dot(N, L));
			const float S = pow(std::max(0.0f, dot(H, N)), 32.0f);
			float dist = pyramid(pf, x, y) * camera.getFar();
			dist = 0.333f * dist * dist;
			vec3 color = ambient + (D * base_color + S * light_color * base_color) / dist;
			color = pow(color, vec3(1.0f/2.2f));
			screen.setPixel(r*WIDTH + c, color);
		}
		
		screen.draw();
        glfwSwapBuffers(window.getWindow());    	
        glClear(GL_COLOR_BUFFER_BIT);
    	screen.clear();
    	pyramid.clear(1.0);
    	
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

