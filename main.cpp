#include "myglheaders.h"
#include "octree.h"
#include <iostream>
#include "time.h"
#include <random>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"

using namespace std;

int main(){
	Window window(1600, 900, 3, 3, "Octree Testing");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	
	prog.bind();
	prog.setUniform("ambient", {0.1f, 0.1f, 0.1f});
	prog.setUniform("light_color", {1.0f, 1.0f, 1.0f});
	prog.setUniformFloat("shininess", 64.0f);
	
    PointBuffer pb;
    pb.reserve(314*628);
    for(float theta = 0.0f; theta < 3.14f; theta += 0.01f){
    	for(float phi = 0.0f; phi < 6.28f; phi += 0.01f){
    		float x = cos(theta) * sin(phi);
    		float z = sin(theta) * sin(phi);
    		float y = cos(phi);
				glm::vec3 pos(x, y, z);
				glm::vec3 normal = glm::normalize(pos);
				pb.push_back(Point(pos, normal, {1.0f, 1.0f, 1.0f}, 10.0f));
		}
	}
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
	PointMesh pointMesh;
	pointMesh.upload(pb);
	pb.clear();
	
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
		pointMesh.draw();
		
        glfwSwapBuffers(window.getWindow());
        i++;
        if(i >= 60){
            i = 0;
            printf("FPS: %f\n", 60.0 / t);
            t = 0.0;
            glfwSetTime(0.0);
        }
    }
    return 0;
}
