#include "myglheaders.h"
#include "interval.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "glscreen.h"
#include "omp.h"

using namespace std;
using namespace glm;

#define WIDTH 640
#define HEIGHT 360

static float depths[HEIGHT][WIDTH];

int clamp(int a, int l, int h){
	return std::max(std::min(a, h), l);
}

vec2 map(const mat3& p){
	return isphere(p, {0.0f, 0.0f, 0.0f}, 1.0f);
}

vec2 trace(const vec3& ro, const vec3& rd, vec2 t){
	return map(iadd(ro, imul(rd, t)));
}

vec3* trace(const vec3& ro, const vec3& rd, float e){
	const float zmax = 100.0f;
	vec2 t = vec2(0.1f, zmax);
	if(!icontains(trace(ro, rd, t), 0.0f)) return nullptr;
	for(int i = 1; i < 60; i++){
		float th = icenter(t);
		vec2 t0 = vec2(t.x, th);
		vec2 d = trace(ro, rd, t0);
		if(icontains(d, 0.0f)){
			t.y = th;
			if(std::max(abs(d.x), abs(d.y)) < e)
				return new vec3(ro + rd * icenter(t));
			continue;
		}
		t0 = vec2(th, t.y);
		d = trace(ro, rd, t0);
		if(icontains(d, 0.0f)){
			t.x = th;
			if(std::max(abs(d.x), abs(d.y)) < e)
				return new vec3(ro + rd * icenter(t));
			continue;
		}
		float dd = (t.y - t.x);
		t.x += dd;
		t.y += dd*2.0f;
		if(t.x >= zmax) return nullptr;
	}
	return nullptr;
}

int main(){
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	camera.setEye({0.0f, 0.0f, 1.0f});
	camera.update(); 
	
	GLScreen screen(WIDTH, HEIGHT);
	
	prog.bind();
    
    const float dx = 2.0f / (float)WIDTH;
	const float dy = 2.0f / (float)HEIGHT;
	
	vec3 ambient(0.001f, 0.0005f, 0.0005f);
    vec3 light_pos(0.5f, 1.0f, 3.0f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        double dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
        light_pos = camera.getEye();
        
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			const int r = k / WIDTH;
			const int c = k % WIDTH;
			const float x = c * dx - 1.0f;
			const float y = r * dy - 1.0f;
			vec3 ro = camera.getEye();
			vec3 rd = camera.getRay(x, y);
			vec3* pos = trace(ro, rd, 0.01f);
			if(pos == nullptr) continue;
			depths[r][c] = distance(ro, *pos) / 100.0f;
		}
		
		#pragma omp parallel for num_threads(8) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			const int r = clamp(k / WIDTH, 1, HEIGHT-2);
			const int c = clamp(k % WIDTH, 1, WIDTH-2);
			const float x = c * dx - 1.0f;
			const float y = r * dy - 1.0f;
			if(depths[r][c] == 1.0f) continue;
			vec3 pos = vec3(x, y, depths[r][c]);
			vec3 N = normalize(vec3(depths[r][c+1] - depths[r][c-1],
									depths[r+1][c] - depths[r-1][c],
									0.5f));
			vec4 lpos = camera.getV() * vec4(light_pos.xyz(), 1.0f);
			vec3 L = normalize(lpos.xyz() - pos);
			vec3 V = vec3(0.0f, 0.0f, 1.0f);
			vec3 H = normalize(L + V);
			const float D = std::max(0.0f, dot(N, L));
			const float S = pow(std::max(0.0f, dot(H, N)), 16.0f);
			const float dist2 = dot(lpos.xyz() - pos, lpos.xyz() - pos);
			vec3 color = ambient + (D * base_color + S * light_color * base_color) / dist2;
			color = pow(color, vec3(1.0f/2.2f));
			screen.setPixel(r*WIDTH + c, color);
		}
		
		screen.draw();
        glfwSwapBuffers(window.getWindow());    	
        glClear(GL_COLOR_BUFFER_BIT);
    	screen.clear();
    	for(unsigned r = 0; r < HEIGHT; r++){
    	for(unsigned c = 0; c < WIDTH; c++){
    		depths[r][c] = 1.0f;
    	}}
    	
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
