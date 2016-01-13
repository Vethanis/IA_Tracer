
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
	unsigned callsizeX = WIDTH / 8 + (WIDTH % 8) ? 1 : 0;
	unsigned callsizeY = HEIGHT / 8 + (HEIGHT % 8) ? 1 : 0;
	GLProgram colorProg("fullscreen.glsl", "color.glsl");
	GLScreen screen(WIDTH, HEIGHT);
	Texture dbuf(WIDTH, HEIGHT, DEPTH);
	
    vec3 light_pos(1.1f, 1.1f, 1.5f);
    colorProg.bind();
	colorProg.setUniform("ambient", vec3(0.001f, 0.0005f, 0.0005f));
	colorProg.setUniform("light_color", vec3(1.0f));
	colorProg.setUniform("base_color", vec3(0.5f, 0.1f, 0.01f));
	colorProg.setUniform("light_pos", light_pos);
	colorProg.setUniform("ddx", ddx);
	colorProg.setUniform("ddy", ddy);
	colorProg.setUniformFloat("near", camera.getNear());
	colorProg.setUniformFloat("far", camera.getFar());
	colorProg.setUniformInt("dbuf", 0);
	
	depthProg.bind();
	depthProg.setUniformFloat("near", camera.getNear());
	depthProg.setUniformFloat("far", camera.getFar());
	depthProg.setUniformInt("dbuf", 0);
	
	dbuf.bind(0);
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
		
		vec3 tl = camera.getRay(-1.0f,  1.0f);
		vec3 tr = camera.getRay( 1.0f,  1.0f);
		vec3 bl = camera.getRay(-1.0f, -1.0f);
		vec3 br = camera.getRay( 1.0f, -1.0f);
		
		depthProg.bind();
		depthProg.setUniform("eye", camera.getEye());
		depthProg.setUniform("tl", tl);
		depthProg.setUniform("tr", tr);
		depthProg.setUniform("bl", bl);
		depthProg.setUniform("br", br);
		depthProg.call(callsizeX, callsizeY, 1);
		
		colorProg.bind();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			colorProg.setUniform("light_pos", camera.getEye());
		colorProg.setUniform("eye", camera.getEye());
		colorProg.setUniform("tl", tl);
		colorProg.setUniform("tr", tr);
		colorProg.setUniform("bl", bl);
		colorProg.setUniform("br", br);
		screen.draw();
		
        window.swap();
    }
    return 0;
}

