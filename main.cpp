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
#define MAXDEPTH 8

static float depth[HEIGHT][WIDTH];

vec2 map(const mat3& p){
	return isphere(p, vec3(0.0f), 1.0f);
}

vec2* BeamCast(const mat4& IVP, const vec2& X, const vec2& Y, const vec2& Z, const float e){
	vec2 z = Z;
	if(z.y - z.x < e) return new vec2(z);
	for(int i = 0; i < 60; i++){
		//cout << z.x << " " << z.y << endl;
		float zc = icenter(z);
		vec2 z0 = vec2(z.x, zc);
		if(icontains(map(iatransform(IVP, iavec3(X, Y, z0))), 0.0f)){
			if(z0.y - z0.x < e) return new vec2(z0);
			z = z0; continue;
		}
		vec2 z1 = vec2(zc, z.y);
		if(icontains(map(iatransform(IVP, iavec3(X, Y, z1))), 0.0f)){
			if(z1.y - z1.x < e) return new vec2(z1);
			z = z1; continue;
		}
		break;
	}
	if(z.y < Z.y) return new vec2(z);
	return nullptr;
}

vec2 trace(const vec3& ro, const vec3& rd, vec2 t){
	return map(iadd(ro, imul(rd, t)));
}

vec3* trace(const vec3& ro, const vec3& rd, float e){
	const float zmax = 10.0f;
	vec2 t = vec2(0.1f, zmax);
	for(int i = 0; i < 60; i++){
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
		float depth = (t.y - t.x);
		t.x += depth;
		t.y += depth*2.0f;
		if(t.x >= zmax) return nullptr;
	}
	return nullptr;
}

void Paint(const vec2& X, const vec2& Y, float Z){
	float cx = WIDTH * 0.5f;
	float cy = HEIGHT * 0.5f;
	int rmin = (Y.x + 1.0f) * cy;
	int rmax = (Y.y + 1.0f) * cy;
	int cmin = (X.x + 1.0f) * cx;
	int cmax = (X.y + 1.0f) * cx;
	for(int r = rmin; r < rmax; r++){
		for(int c = cmin; c < cmax; c++){
			depth[r][c] = Z;
		}
	}
}

void SubDivide(const mat4& IVP, const vec2& X, const vec2& Y, const float Z, int d, float e){
	vec2* T = BeamCast(IVP, X, Y, vec2(Z, 1.0f), e);
	if(d >= MAXDEPTH){
		float depth = (T) ? icenter(*T) : Z*2.0f;
		Paint(X, Y, depth);
		delete T;
	}
	else if(T == nullptr) return;
	else {
		vec2 x1(X.x, icenter(X));
		vec2 x2(x1.y, X.y);
		vec2 y1(Y.x, icenter(Y));
		vec2 y2(y1.y, Y.y);
		float Z2 = T->x * 0.5f;
		delete T;
		SubDivide(IVP, x1, y1, Z2, d+1, e*0.5f);
		SubDivide(IVP, x1, y2, Z2, d+1, e*0.5f);
		SubDivide(IVP, x2, y1, Z2, d+1, e*0.5f);
		SubDivide(IVP, x2, y2, Z2, d+1, e*0.5f);
	}
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
	
	vec3 light_pos(2.0f, 2.0f, 2.0f);
	vec3 L = normalize(light_pos);
	vec3 ambient(0.1f, 0.05f, 0.05f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        double dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
        
        //SubDivide(camera.getIVP(), {-1.0f, 1.0f}, {-1.0f, 1.0f}, 0.0f, 0, 0.1f);

		#pragma omp parallel for num_threads(4) schedule(dynamic, 24)
		for(int k = 0; k < WIDTH * HEIGHT; k++){
			int r = k / WIDTH;
			int c = k % WIDTH;
			float x = c * dx - 1.0f;
			float y = r * dy - 1.0f;
			vec3 ro = camera.getEye();
			vec3 rd = camera.getRay(x, y);
			vec3* pos = trace(ro, rd, 0.001f);
			if(pos){
				vec3 normal = normalize(*pos);
				vec3 V = normalize(camera.getEye() - *pos);
				vec3 H = normalize(L + V);
				float D = std::max(0.0f, dot(normal, L));
				float S = pow(std::max(0.0f, dot(H, normal)), 16.0f);
				vec3 color = ambient + (D * normal + S * light_color * normal) / distance(light_pos, *pos);
				screen.setPixel(r*WIDTH + c, color);
			}
		}
		
		screen.draw();
        glfwSwapBuffers(window.getWindow());    	
        glClear(GL_COLOR_BUFFER_BIT);
    	screen.clear();
    	for(unsigned r = 0; r < HEIGHT; r++){
    	for(unsigned c = 0; c < WIDTH; c++){
    		depth[r][c] = 1.0;
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
