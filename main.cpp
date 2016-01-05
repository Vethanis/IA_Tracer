#include "octree.h"
#include <iostream>
#include "time.h"
#include <random>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "myglheaders.h"
using namespace std;

int main(){
	Window window(800, 600, 3, 3, "Octree Testing");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	prog.bind();
	prog.setUniform("ambient", {0.05f, 0.0f, 0.0f});
	prog.setUniform("light_pos", {5.0f, 5.0f, -5.0f});
	prog.setUniform("light_color", {1.0f, 1.0f, 1.0f});
	prog.setUniform("eye", camera.getEye());
	prog.setUniformFloat("shininess", 16.0f);
    srand(time(NULL));
    OctNode root({0.0f, 0.0f, 0.0f}, 1000.0f);
    for(unsigned i = 0; i < 1000; i++){
        float n[10];
        for(unsigned j = 0; j < 10; j++){
            n[j] = (float)(rand()%2000) - 1000.0f;
            cout << n[j] << " ";
        }
        cout << i << "\n";
        Point* pt = new Point({n[0],n[1],n[2]}, {n[3],n[4],n[5]}, {n[6],n[7],n[8]}, n[9]);
        root.insert(pt);
    }
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        double dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
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
