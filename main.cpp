#include "myglheaders.h"
#include "octree.h"
#include <iostream>
#include "time.h"
#include <random>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "interval3.h"
#include "omp.h"

using namespace std;
using namespace glm;

#define WIDTH 800
#define HEIGHT 600
#define MAXDEPTH 64

static float depth[HEIGHT][WIDTH];

vec2* BeamCast(const mat4& IVP, const vec2& X, const vec2& Y, const vec2& Z, const float e){
	Interval3* box = new Interval3(X, Y, Z);	// make an interval AABB
	box->transform(IVP);	// transform to world space
	vec2 a = IASphere(*box, {0.0f, 0.0f, 0.0f}, 1.0f);	// get base interval
	vec2 wz(box->min.z, box->max.z);
	printf("Z: [%.3f, %.3f], WZ: [%.3f, %.3f], F(B): [%.3f, %.3f]\n", Z.x, Z.y, wz.x, wz.y, a.x, a.y);
	delete box;
	if(a.x > 0.0f || a.y < 0.0f) return nullptr;	// no surface in interval
	else {
		if(Z.y - Z.x < e) return new vec2(Z);	// interval within threshold
		else {
			a = vec2(Z.x, (Z.x+Z.y) * 0.5f);
			vec2* J = BeamCast(IVP, X, Y, a, e);	// test near bisection
			if(J)
				return J;
			else{
				a = vec2(a.y, Z.y);
				return BeamCast(IVP, X, Y, a, e); // test far bisection
			}
		}
	}
}

void Paint(const vec2& X, const vec2& Y, vec2* T){
	if(T){
		for(int y = Y.x; y < Y.y; y++){
			for(int x = X.x; x < X.y; x++){
				depth[y][x] = 0.5f*(T->x + T->y);
			}
		}
	}
	else{
		for(int y = Y.x; y < Y.y; y++){
			for(int x = X.x; x < X.y; x++){
				depth[y][x] = -100.0f;
			}
		}
	}
}

void SubDivide(const mat4& IVP, const vec2& X, const vec2& Y, float w, int d, float e){
	vec2* T = BeamCast(IVP, X, Y, vec2(w, 1.0f), e);
	if(T == nullptr || d == MAXDEPTH)	// no result or at max depth
		Paint(X, Y, T);
	else {
		vec2 x1(X.x, 0.5f*(X.x+X.y));
		vec2 x2(x1.y, X.y);
		vec2 y1(Y.x, 0.5f*(Y.x+Y.y));
		vec2 y2(y1.x, Y.y);
		float a = std::min(T->x, T->y);
		SubDivide(IVP, x1, y1, a, d+1, e*0.5f);
		SubDivide(IVP, x1, y2, a, d+1, e*0.5f);
		SubDivide(IVP, x2, y1, a, d+1, e*0.5f);
		SubDivide(IVP, x2, y2, a, d+1, e*0.5f);
	}
}

int main(){
	Window window(WIDTH, HEIGHT, 3, 3, "Octree Testing");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	camera.setEye({0.0f, 0.0f, 50.0f});
	camera.update(); 
	
	vec2* asdf = BeamCast(camera.getIVP(), vec2(-1.0f, 1.0f), vec2(-1.0f, 1.0f), vec2(-1.0f, 1.0f), 0.001f);
	delete asdf;
	return 0;
	
	glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
	PointMesh pointMesh;
	
	prog.bind();
	prog.setUniform("light_pos", camera.getEye());
	prog.setUniform("ambient", {0.001f, 0.001f, 0.001f});
	prog.setUniform("light_color", {1.0f, 1.0f, 1.0f});
	prog.setUniformFloat("shininess", 64.0f);
	
    PointBuffer pb;
    pb.reserve(1024);
    
    const float dx = 2.0f / (float)WIDTH;
	const float dy = 2.0f / (float)HEIGHT;
	
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
    	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        double dt = glfwGetTime() - t;
        t += dt;
        
        input.poll(dt, camera);
        prog.setUniform("VP", camera.getVP());
        prog.setUniform("eye", camera.getEye());
        if(glfwGetKey(window.getWindow(), GLFW_KEY_F))
			prog.setUniform("light_pos", camera.getEye());
		
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E)){
			#pragma omp parallel for num_threads(4)
			for(int k = 0; k < WIDTH * HEIGHT; k++){
				int r = k / WIDTH;
				int c = k % WIDTH;
				float x = -1.0f + c * dx;
				float y = -1.0f + r * dy;
				vec2* di = BeamCast(camera.getIVP(), vec2(x, x+dx), vec2(y, y+dy), vec2(-1.0f, 1.0f), 0.001f);
				if(di){
					float z = (di->x + di->y) * 0.5f;
					delete di;
					vec3 pos = camera.getPoint(x, y, z);
					vec3 normal = normalize(pos);
					#pragma omp critical
					{ 
						pb.push_back(Point(pos, normal, normal, 2.0f));
					}
				}
			}

			pointMesh.upload(pb);
			pb.clear();
		}
		
		pointMesh.draw();
		
        glfwSwapBuffers(window.getWindow());
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
