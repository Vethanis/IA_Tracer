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
	prog.setUniform("light_pos", {10.0f, 10.0f, 10.0f});
	prog.setUniform("light_color", {1.0f, 1.0f, 1.0f});
	
	prog.setUniformFloat("shininess", 16.0f);
	
	
    srand(time(NULL));
    OctNode root({0.0f, 0.0f, 0.0f}, 100.0f);
    PointBuffer pb;
    float r = 2.0f;
	for(float j = 0.0f; j < 3.14f; j += 0.001f){
		float y = r * cos(j);
		for(float k = 0.0f; k < 6.28f; k+= 0.002f){
			float x = r * sin(j) * sin(k);
			float z = r * sin(j) * cos(k);
		    glm::vec3 pos(x, y, z);
		    glm::vec3 normal = -1.0f * glm::normalize(pos);
		    pb.push_back(Point(pos, normal, {1.0f, 1.0f, 1.0f}, 10.0f));
		    //root.insert(pt);
    }}
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
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
