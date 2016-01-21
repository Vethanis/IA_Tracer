
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
#include "UBO.h"
#include "timer.h"

//#include "test.h"

#include <random>
#include "time.h"

using namespace std;
using namespace glm;

#define NUM_PRIMS 200

struct CommonParams{
mat4 IVP;
vec4 eye;
vec4 nfp;	// near, far
ivec4 whnp; // width, height, num_prims, max_depth
	CommonParams(Camera& cam, int width, int height, int nprims)
		: IVP(cam.getIVP()), eye(vec4(cam.getEye(), 1.0f)),
		nfp(vec4(cam.getNear(), cam.getFar(), cam.getAR(), cam.getFov())),
		whnp(ivec4(width, height, nprims, 0)){
			int md = 0;
			int w = glm::min(width, height);
			while(w > 0){
				w = w >> 1;
				md++;
			}
			printf("%i\n", md);
			whnp.w = md;
		};
	void update(Camera& cam){
		IVP = cam.getIVP();
		eye = vec4(cam.getEye(), 0.0f);
	}
};

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
	
	srand(time(NULL));
	Camera camera;
	const int WIDTH = atoi(argv[1]);
	const int HEIGHT = atoi(argv[2]);
	vec2 ddx(2.0f/WIDTH, 0.0f);
	vec2 ddy(0.0f, 2.0f/HEIGHT);
	unsigned layoutSize = 8;
	unsigned callsizeX = WIDTH / layoutSize + ((WIDTH % layoutSize) ? 1 : 0);
	unsigned callsizeY = HEIGHT / layoutSize + ((HEIGHT % layoutSize) ? 1 : 0);
	//camera.setEye({0.0f, 0.0f, 2.0f});
	camera.setEye({0.0f, 2.f, -.5f});
	camera.pitch(-90.0f);
	camera.setFov(90.0f);
	camera.setPlanes(0.1f, 400.0f);
	//camera.setPlanes(0.1f, 50.0f);
	camera.resize(WIDTH, HEIGHT);
	camera.update();
	
	Window window(WIDTH, HEIGHT, 4, 3, "");
	Input input(window.getWindow());
	GLScreen screen;
	//ComputeShader rayProg("rmarch.glsl");
	ComputeShader depthProg("depth.glsl");
	ComputeShader clearProg("clear.glsl");
	GLProgram colorProg("fullscreen.glsl", "color.glsl");
	Texture dbuf(WIDTH, HEIGHT, FLOAT2);
	dbuf.setCSBinding(0);
	/*CSGParam params[NUM_PRIMS];
	for(int i = 0; i < NUM_PRIMS; i++){
		float a[6];
		for(int j = 0; j < 3;j++)
			a[j] = (rand()%2000 - 1000.0f) / 200.0f;
		for(int j = 3; j < 6; j++)	// [0,100]
			a[j] = 0.1f + ((rand()%300) / 100.0f);
		float t = (float)(rand()%2);
		float m = (float)i / NUM_PRIMS;
		params[i] = CSGParam({a[0], a[1], a[2]}, {a[3], a[4], a[5]}, t, m);
	}
	SSBO paramssbo(&params[0], sizeof(params), 1);
	*/
	CommonParams com_p(camera, WIDTH, HEIGHT, NUM_PRIMS);
	UBO camUBO(&com_p, sizeof(com_p), 2);
	
    vec3 light_pos(5.0f, 5.0f, 5.0f);
    colorProg.bind();
	colorProg.setUniform("ambient", vec3(0.0001f, 0.00005f, 0.00005f));
	colorProg.setUniform("light_color", vec3(1.0f));
	colorProg.setUniform("light_pos", light_pos);
	colorProg.setUniform("ddx", ddx);
	colorProg.setUniform("ddy", ddy);
	colorProg.setUniformFloat("light_str", 100.0f);
	
	Timer timer;
	
	input.poll();
    unsigned i = 0;
    double t = glfwGetTime();
    while(window.open()){
        input.poll(frameBegin(i, t), camera);
        com_p.update(camera);
        camUBO.upload(&com_p, sizeof(com_p));
		
		depthProg.bind();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		timer.begin();
		depthProg.call(callsizeX, callsizeY, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		timer.endPrint();
		
		//test(camera, dbuf, ivec2(WIDTH, HEIGHT));
		
		colorProg.bind();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			colorProg.setUniform("light_pos", camera.getEye());
		dbuf.bind(0, "dbuf", colorProg);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		screen.draw();
		
        clearProg.bind();
        clearProg.call(callsizeX, callsizeY, 1);
        window.swap();
    }
    return 0;
}

