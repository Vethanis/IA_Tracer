
#define GLM_FORCE_AVX
#include "myglheaders.h"
#include "debugmacro.h"
#include "glm/gtx/simd_vec4.hpp"
#include "glm/gtx/simd_mat4.hpp"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "glscreen.h"

using namespace std;
using namespace glm;

double frameBegin(unsigned& i, double& t){
    double dt = glfwGetTime() - t;
    t += dt;
    i++;
    if(t >= 3.0){
        printf("ms: %.6f, FPS: %.3f\n", t / i, i / t);
        i = 0;
        t = 0.0;
        glfwSetTime(0.0);
    }
    return dt;
}

int main(int argc, char* argv[]){
	if(argc < 2){
		cout << argv[0] << " <screen width> <screen height>" << endl;
		return 1;
	}
	Camera camera;
	const int WIDTH = atoi(argv[1]);
	const int HEIGHT = atoi(argv[2]);
	const float dx = 2.0f / WIDTH;
	const float dy = 2.0f / HEIGHT;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WIDTH, HEIGHT);
	camera.update();
	
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram depthProg("fullscreen.vert", "depth.frag");
	GLProgram colorProg("fullscreen.vert", "color.frag");
	GLScreen screen(WIDTH, HEIGHT);
	
    vec3 light_pos(1.1f, 1.1f, 1.5f);
	colorProg.setUniform("ambient", vec3(0.001f, 0.0005f, 0.0005f));
	colorProg.setUniform("light_color", vec3(1.0f));
	colorProg.setUniform("base_color", vec3(0.5f, 0.1f, 0.01f));
	colorProg.setUniform("light_pos", light_pos);
	colorProg.setUniformFloat("dx", dx);
	colorProg.setUniformFloat("dy", dy);
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.alive()){
        input.poll(frameBegin(i, t), camera);
		
		depthProg.bind();
		depthProg.setUniform("IVP", camera.getIVP());
		screen.draw();
		
		colorProg.bind();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			colorProg.setUniform("light_pos", camera.getEye());
		colorProg.setUniform("IVP", camera.getIVP());
		colorProg.setUniform("eye", camera.getEye());
		screen.draw();
		
        window.swap();
    }
    return 0;
}

