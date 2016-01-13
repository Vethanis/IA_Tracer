#include "myglheaders.h"
#include "ival.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "glscreen.h"
#include "glm/gtx/compatibility.hpp"
#include "glmprint.h"
#include "frustum.h"
#include <string>
#include "omp.h"

using namespace std;
using namespace glm;

#define NEAR 0.1f
#define FAR 20.0f

struct SubArgs{
	ival3 uvt;
	float e;
	int depth;
	SubArgs(const ival3& a, float b, int c)
		: uvt(a), e(b), depth(c){};
};

inline ival map(const ival3& p){
	return isphere(p, {0.0f, 0.0f, 0.0f}, 1.0f);
}

// returns ival in [0, 1] range for z
ival* trace(const Camera& c, const SubArgs& args){
	ival t = args.uvt.z;
	for(int i = 0; i < 32; i++){
		ival2 ts = split(t);
		ival d = map(getInterval(c, args.uvt, ts.x));
		if(contains(d, 0.0f)){
			t = ts.x;
			if(width(t) < args.e)
				return new ival(t);
			continue;
		}
		d = map(getInterval(c, args.uvt, ts.y));
		if(contains(d, 0.0f)){
			t = ts.y;
			if(width(t) < args.e)
				return new ival(t);
			continue;
		}
		widen(t, args.e);
		if(t.h > 1.0f || t.l < 0.0f) return nullptr;
	}
	return nullptr;
}

void split(std::vector<SubArgs>& s, const SubArgs& args){
	ival2 u = split(args.uvt.x);
	ival2 v = split(args.uvt.y);
	s.push_back({ival3(u.x, v.x, args.uvt.z), args.e*0.5f, args.depth+1});
	s.push_back({ival3(u.x, v.y, args.uvt.z), args.e*0.5f, args.depth+1});
	s.push_back({ival3(u.y, v.x, args.uvt.z), args.e*0.5f, args.depth+1});
	s.push_back({ival3(u.y, v.y, args.uvt.z), args.e*0.5f, args.depth+1});
}

void subdivide(const Camera& cam, GLScreen& buf, const ival2& uv, int start_depth, float e){
	int depth = start_depth;
	const int max_depth = 8;
	vector<SubArgs> stack;
	stack.reserve(4);
	stack.push_back({{uv.x, uv.y, {0.0f, 1.0f}}, e, depth});
	while(!stack.empty()){
		SubArgs args = stack.back();
		stack.pop_back();
		ival *t = trace(cam, args);
		if(t){
			if(args.depth < max_depth){
				args.uvt.z = *t;
				split(stack, args);
			}
			else
				buf.paint(args.uvt);
			delete t;
		}
	}
}

int main(int argc, char* argv[]){
	if(argc < 3){
		cout << argv[0] << " <screen width> <screen height> <num_threads>" << endl;
		return 1;
	}
	const int WIDTH = atoi(argv[1]);
	const int HEIGHT = atoi(argv[2]);
	const unsigned threads = stoul(argv[3]);
	const unsigned regions = threads;
	float ratio = (float)WIDTH / (float)HEIGHT;
	int start_depth = 0;
	std::vector<ival2> uvs;
	getStartingUVs(ratio, regions, uvs, start_depth);

	const float dx = 2.0f / WIDTH;
	const float dy = 2.0f / HEIGHT;
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram depthProg("fullscreen.vert", "depth.frag");
	GLProgram colorProg("fullscreen.vert", "color.frag");
	GLScreen screen(WIDTH, HEIGHT);
	
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WIDTH, HEIGHT);
	camera.update();
    
    vec3 light_pos(1.1f, 1.1f, 1.5f);
    
	colorProg.setUniform("ambient", vec3(0.001f, 0.0005f, 0.0005f));
	colorProg.setUniform("light_color", vec3(1.0f));
	colorProg.setUniform("base_color", vec3(0.5f, 0.1f, 0.01f));
	colorProg.setUniform("light_pos", light_pos);
	colorProg.setUniformFloat("dx", dx);
	colorProg.setUniformFloat("dy", dy);
	
	input.poll();
    unsigned i = 0;
    float t = glfwGetTime();
    while(window.alive()){
        float dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
		
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
    	
        i++;
        if(i >= 60){
            i = 0;
            printf("ms: %f\n", t / 60.0);
            t = 0.0;
            glfwSetTime(0.0);
        }
    }
    return 0;
}

