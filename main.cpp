
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

struct CSGParam{
	vec4 center;
	vec4 dim;
	CSGParam() : center(vec4(0.0f, 0.0f, 0.0f, -1.0f)), dim(vec4(0.0f, 0.0f, 0.0f, -1.0f)){};
	CSGParam(const vec3& c, const vec3& d, float type, float mat){
		center = vec4(c, type); dim = vec4(d, mat);
	}
	CSGParam(const CSGParam& other)
		: center(other.center), dim(other.dim){};
};

CSGParam params[2];

double frameBegin(unsigned& i, double& t){
    double dt = glfwGetTime() - t;
    t += dt;
    i++;
    if(t >= 3.0){
    	double ms = (t / i) * 1000.0;
        printf("ms: %.6f, FPS: %.3f\n", ms, i / t);
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
	camera.setEye({0.0f, 0.f, 2.f});
	camera.setPlanes(0.1f, 100.0f);
	camera.resize(WIDTH, HEIGHT);
	camera.update();
	
	Window window(WIDTH, HEIGHT, 4, 3, "IA Ray Casting");
	Input input(window.getWindow());
	ComputeShader rayProg("rmarch.glsl");
	ComputeShader clearProg("clear.glsl");
	unsigned callsizeX = WIDTH / 8 + ((WIDTH % 8) ? 1 : 0);
	unsigned callsizeY = HEIGHT / 8 + ((HEIGHT % 8) ? 1 : 0);
	GLProgram colorProg("fullscreen.glsl", "color.glsl");
	GLScreen screen;
	Texture dbuf(WIDTH, HEIGHT, FLOAT);
	dbuf.setCSBinding(0);
	params[0] = CSGParam(vec3(1.0f), vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f);
	params[1] = CSGParam(vec3(0.0f), vec3(0.5f, 0.5f, 0.5f), 1.0f, 1.0f);
	SSBO paramssbo(&params[0], sizeof(params), 1);
	
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
	
	rayProg.bind();
	rayProg.setUniformFloat("near", camera.getNear());
	rayProg.setUniformFloat("far", camera.getFar());
	
	Timer timer;
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
		
		rayProg.bind();
		rayProg.setUniform("IVP", camera.getIVP());
		rayProg.setUniform("eye", camera.getEye());
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		rayProg.call(callsizeX, callsizeY, 1);
		
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

