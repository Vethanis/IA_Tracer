
#include "myglheaders.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "glscreen.h"
#include "texture.h"
#include "compute_shader.h"
#include "SSBO.h"
#include "timer.h"

using namespace std;
using namespace glm;

struct TPyramid{
	// validity of a t is given by (t[id].x < 0.0f)
	// all valid ranges of t are with in [0, 1]
	// should have log2(max(width, height))-1 levels.
	// final level is the depth texture
	vec2 t9[4*4*4*4*4*4*4*4*4];
	vec2 t8[4*4*4*4*4*4*4*4];
	vec2 t7[4*4*4*4*4*4*4];
	vec2 t6[4*4*4*4*4*4];
	vec2 t5[4*4*4*4*4];
	vec2 t4[4*4*4*4];
	vec2 t3[4*4*4];
	vec2 t2[4*4];
	vec2 t1[4];
	vec2 t0;
	void clear(){
		t0 = vec2(0.0f, 1.0f);
		vec2 c(-1.0f, 1.0f);
		for(auto& i : t1) i = c;
		for(auto& i : t2) i = c;
		for(auto& i : t3) i = c;
		for(auto& i : t4) i = c;
		for(auto& i : t5) i = c;
		for(auto& i : t6) i = c;
		for(auto& i : t7) i = c;
		for(auto& i : t8) i = c;
		for(auto& i : t9) i = c;
	}
	int uvToIndex(vec2 uv, int depth){
		unsigned size = 1;
		size = size << depth;
		int r = (0.5f * uv.x + 0.5f) * size;
		int c = (0.5f * uv.y + 0.5f) * size;
		return r + size * c;
	}
	vec4 IndexToUV(int i, int depth){
		unsigned size = 1;
		size = size << depth;
		int r = i % size;
		int c = i / size;
		float dx = 2.0f / size;
		float dy = 2.0f / size;
		float u = -1.0f + c * dx;
		float v = -1.0f + r * dy;
		return vec4(u, u+dx, v, v+dy);
	}
};

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
	camera.setEye({0.0f, 2.f, -.5f});
	camera.resize(WIDTH, HEIGHT);
	camera.setFov(90.0f);
	camera.setPlanes(0.1f, 400.0f);
	camera.pitch(-90.0f);
	camera.update();
	
	Window window(WIDTH, HEIGHT, 4, 3, "IA Ray Casting");
	Input input(window.getWindow());
	ComputeShader depthProg("depth.glsl");
	ComputeShader clearProg("clear.glsl");
	unsigned callsizeX = WIDTH / 8 + ((WIDTH % 8) ? 1 : 0);
	unsigned callsizeY = HEIGHT / 8 + ((HEIGHT % 8) ? 1 : 0);
	GLProgram colorProg("fullscreen.glsl", "color.glsl");
	GLScreen screen;
	Texture dbuf(WIDTH, HEIGHT, FLOAT);
	dbuf.setCSBinding(0);
	
    vec3 light_pos(3.0f, 3.0f, 3.0f);
    colorProg.bind();
	colorProg.setUniform("ambient", vec3(0.001f, 0.0005f, 0.0005f));
	colorProg.setUniform("light_color", vec3(1.0f));
	colorProg.setUniform("base_color", vec3(0.7f, 0.1f, 0.01f));
	colorProg.setUniform("light_pos", light_pos);
	colorProg.setUniform("ddx", ddx);
	colorProg.setUniform("ddy", ddy);
	colorProg.setUniformFloat("near", camera.getNear());
	colorProg.setUniformFloat("far", camera.getFar());
	colorProg.setUniformFloat("light_str", 10.0f);
	
	Timer timer;
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
		
		depthProg.bind();
		depthProg.setUniform("IVP", camera.getIVP());
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		if(t > 2.9)timer.begin();
		depthProg.call(callsizeX, callsizeY, 1);
		if(t > 2.9)timer.endPrint();
		
		colorProg.bind();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			colorProg.setUniform("light_pos", camera.getEye());
		colorProg.setUniform("IVP", camera.getIVP());
		colorProg.setUniform("eye", camera.getEye());
		dbuf.bind(0);
		colorProg.setUniformInt("dbuf", 0);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		screen.draw();
		
        window.swap();
        clearProg.bind();
        clearProg.call(callsizeX, callsizeY, 1);
        
    }
    return 0;
}

