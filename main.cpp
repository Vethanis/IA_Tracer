
#include "myglheaders.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "glscreen.h"
#include "texture.h"
#include "compute_shader.h"

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
	if(argc != 3){
		cout << argv[0] << " <screen width> <screen height>" << endl;
		return 1;
	}
	Camera camera;
	const int WIDTH = atoi(argv[1]);
	const int HEIGHT = atoi(argv[2]);
	vec2 ddx(2.0f/WIDTH, 0.0f);
	vec2 ddy(0.0f, 2.0f/HEIGHT);
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WIDTH, HEIGHT);
	camera.update();
	
	Window window(WIDTH, HEIGHT, 4, 3, "IA Ray Casting");
	Input input(window.getWindow());
	ComputeShader depthProg("depth.glsl");
	ComputeShader clearProg("clear.glsl");
	unsigned callsizeX = WIDTH / 8 + ((WIDTH % 8) ? 1 : 0);
	unsigned callsizeY = HEIGHT / 8 + ((HEIGHT % 8) ? 1 : 0);
	GLProgram colorProg("fullscreen.glsl", "color.glsl");
	GLScreen screen(WIDTH, HEIGHT);
	Texture dbuf(WIDTH, HEIGHT, DEPTH);
	glBindImageTexture(0, dbuf.getID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	
    vec3 light_pos(10.5f, 10.5f, 10.5f);
    colorProg.bind();
	colorProg.setUniform("ambient", vec3(0.001f, 0.0005f, 0.0005f));
	colorProg.setUniform("light_color", vec3(1.0f));
	colorProg.setUniform("base_color", vec3(0.3f, 0.2f, 0.7f));
	colorProg.setUniform("light_pos", light_pos);
	colorProg.setUniform("ddx", ddx);
	colorProg.setUniform("ddy", ddy);
	colorProg.setUniformFloat("light_str", 20.0f);
	
	depthProg.bind();
	depthProg.setUniformFloat("near", camera.getNear());
	depthProg.setUniformFloat("far", camera.getFar());
	
	cout << callsizeX << endl;
	cout << callsizeY << endl;

	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
		
		depthProg.bind();
		depthProg.setUniform("IVP", camera.getIVP());
		depthProg.setUniform("eye", camera.getEye());
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		depthProg.call(callsizeX, callsizeY, 1);
		
		
		colorProg.bind();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			colorProg.setUniform("light_pos", camera.getEye());
		colorProg.setUniform("IVP", camera.getIVP());
		colorProg.setUniform("eye", camera.getEye());
		dbuf.bind(0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		colorProg.setUniformInt("dbuf", 0);
		screen.draw();
		
        window.swap();
        clearProg.bind();
        clearProg.call(callsizeX, callsizeY, 1);
    }
    return 0;
}

