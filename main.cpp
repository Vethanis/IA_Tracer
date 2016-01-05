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
	Window window(800, 600, 3, 3, "Octree Testing");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	
	prog.bind();
	prog.setUniform("ambient", {0.1f, 0.1f, 0.1f});
	prog.setUniform("light_pos", {0.0f, 0.0f, 1.0f});
	prog.setUniform("light_color", {1.0f, 1.0f, 1.0f});
	
	prog.setUniformFloat("shininess", 16.0f);
	
	
    srand(time(NULL));
    OctNode root({0.0f, 0.0f, 0.0f}, 100.0f);
    PointBuffer pb;
    for(unsigned i = 0; i < 100000; i++){
        float n[10];
        for(unsigned j = 0; j < 3; j++){
            n[j] = (float)(rand()%10000)/5000.0f - 1.0f;
        }
        for(unsigned j = 6; j < 9; j++){
            n[j] = (float)(rand()%1024)/1024.0f + 0.1f;
        }
        n[9] = (float)(rand()&1000)/10.0f;
        Point* pt = new Point({n[0], n[1], n[2]}, {0.0f, 0.0f, -1.0f}, {n[6],n[7],n[8]}, n[9]);
        pb.push_back(*pt);
        root.insert(pt);
    }
    glEnable(GL_PROGRAM_POINT_SIZE);
    //root.print(0);
	PointMesh pointMesh;
	pointMesh.upload(pb);
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
    	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        double dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
        prog.setUniform("VP", camera.getVP());
        prog.setUniform("normMat", glm::inverse(glm::transpose(glm::mat3(camera.getV()))));
        prog.setUniform("eye", camera.getEye());
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
