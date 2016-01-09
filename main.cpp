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

Interval* BeamCast(const mat4& IVP, const Interval& X, const Interval& Y, const Interval& Z, const float e){
	Interval3* box = new Interval3(X, Y, Z);	// make an interval AABB
	box->transform(IVP);	// transform to world space
	Interval a = IASphere(*box, {0.0f, 0.0f, 0.0f}, 1.0f);	// get base interval
	//printf("Z: [%f, %f], B.z: [%.3f, %.3f], F(B): [%.3f, %.3f]\n", Z.min, Z.max, box->z.min, box->z.max, a.min, a.max);
	delete box;
	if(a.min > 0.0f || a.max < 0.0f) return nullptr;	// no surface in interval
	else {
		if(Z.width() < e) return new Interval(Z);	// interval within threshold
		else {
			a.min = Z.min; a.max = Z.center();
			Interval* J = BeamCast(IVP, X, Y, a, e);	// test near bisection
			if(J)
				return J;
			else{
				a.min = a.max; a.max = Z.max;
				return BeamCast(IVP, X, Y, a, e); // test far bisection
			}
		}
	}
}

void Paint(const Interval& X, const Interval& Y, Interval* T){
	float dy = 2.0f / WIDTH;
	float dx = 2.0f / HEIGHT;
	float cx = WIDTH * 0.5f;
	float cy = HEIGHT * 0.5f;
	if(T){
		for(float y = Y.min; y < Y.max; y += dy){
			for(float x = X.min; x < X.max; x += dx){
				int c = (x+1.0f) * cx;
				int r = (y+1.0f) * cy;
				c = std::min(std::max(0, c), WIDTH);
				r = std::min(std::max(0, r), HEIGHT);
				depth[r][c] = T->center();
			}
		}
	}
	else{
		for(float y = Y.min; y < Y.max; y += dy){
			for(float x = X.min; x < X.max; x += dx){
				int c = (x+1.0f) * cx;
				int r = (y+1.0f) * cy;
				c = std::min(std::max(0, c), WIDTH);
				r = std::min(std::max(0, r), HEIGHT);
				depth[r][c] = -100.0f;
			}
		}
	}
}

void SubDivide(const mat4& IVP, const Interval& X, const Interval& Y, float w, int d, float e){
	Interval* T = BeamCast(IVP, X, Y, Interval(w, 1.0f), e);
	if(T == nullptr || d == MAXDEPTH)	// no result or at max depth
		Paint(X, Y, T);
	else {
		Interval x1(X.min, X.center());
		Interval x2(x1.max, X.max);
		Interval y1(Y.min, Y.center());
		Interval y2(y1.max, Y.max);
		float z = std::min(T->min, T->max);
		SubDivide(IVP, x1, y1, z, d+1, e*0.5f);
		SubDivide(IVP, x1, y2, z, d+1, e*0.5f);
		SubDivide(IVP, x2, y1, z, d+1, e*0.5f);
		SubDivide(IVP, x2, y2, z, d+1, e*0.5f);
	}
}

int main(){
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	camera.setEye({0.0f, 0.0f, 5.0f});
	camera.update(); 
	
	
	glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
	PointMesh pointMesh;
	
	prog.bind();
	
    PointBuffer pb;
    pb.reserve(1024);
    
    const float dx = 2.0f / (float)WIDTH;
	const float dy = 2.0f / (float)HEIGHT;
	
	vec3 light_pos(1.0f, 1.0f, 1.0f);
	vec3 ambient(0.07f, 0.05f, 0.05f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
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
				Interval* di = BeamCast(camera.getIVP(), {x, x+dx}, {y, y+dy}, {-1.0f, 1.0f}, 0.01f);
				if(di){
					float z = di->center();
					delete di;
					vec3 pos = camera.getPoint(x, y, z);
					vec3 normal = normalize(pos);
					vec3 V = normalize(camera.getEye() - pos);
					vec3 L = normalize(light_pos - pos);
					vec3 H = normalize(L + V);
					float D = std::max(0.0f, dot(normal, L));
					float S = pow(std::max(0.0f, dot(H, normal)), 32.0f);
					vec3 color = ambient + (D * base_color + S * light_color * base_color) / distance(light_pos, pos);
					#pragma omp critical
					{ 
						pb.push_back(Point(pos, normal, color, 5.0f));
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
