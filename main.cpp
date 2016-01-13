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
#include "depthbuffer.h"
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
		if(d.contains(0.0f)){
			t = ts.x;
			if(t.width() < args.e)
				return new ival(t);
			continue;
		}
		d = map(getInterval(c, args.uvt, ts.y));
		if(d.contains(0.0f)){
			t = ts.y;
			if(t.width() < args.e)
				return new ival(t);
			continue;
		}
		t.widen(args.e);
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

void subdivide(unsigned p, const Camera& cam, DepthBuffer& buf, const ival2& uv, int start_depth, float e){
	int depth = start_depth;
	const int max_depth = 8;
	vector<SubArgs> stack;
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

inline float distSquared(const glm::vec3& a, const glm::vec3& b){
	return glm::dot(b - a, b - a);
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
	int start_depth = 0;
	std::vector<ival2> uvs;
	getStartingUVs(regions, uvs, start_depth);

	const float dx = 2.0f / WIDTH;
	const float dy = 2.0f / HEIGHT;
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	prog.bind();
	GLScreen screen(WIDTH, HEIGHT);
	DepthBuffer zbuf(WIDTH, HEIGHT);
	
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WIDTH, HEIGHT);
	camera.update();
    
	vec3 ambient(0.001f, 0.0005f, 0.0005f);
    vec3 light_pos(2.f, 2.f, 2.f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
	
	input.poll();
    unsigned i = 0;
    float t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
        float dt = glfwGetTime() - t;
        t += dt;
        input.poll(dt, camera);
		const vec3 ro = camera.getEye();
		if(glfwGetKey(window.getWindow(), GLFW_KEY_E))
			light_pos = ro;

		#pragma omp parallel for num_threads(threads) schedule(dynamic, 1)
		for(unsigned k = 0; k < regions; k++){
			subdivide(threads, camera, zbuf, uvs[k], start_depth, 0.5f);
		}
		
		#pragma omp parallel for num_threads(threads) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH*HEIGHT; k++){
			const int c = clamp(k % WIDTH, 1, WIDTH-2);
			const int r = clamp(k / HEIGHT, 1, HEIGHT-2);
			const float x = c*dx - 1.0f;
			const float y = r*dy - 1.0f;
			const float z = zbuf[k];
			if(z == 1.0f) continue;
			vec3 pos = camera.getPoint(x, y, z);
			vec3 xv = camera.getPoint(x+dx, y, zbuf[k+1]) - pos;
			vec3 yv = camera.getPoint(x, y+dy, zbuf[k+WIDTH]) - pos;
			vec3 N = normalize(cross(xv, yv));
			vec3 L = normalize(light_pos - pos);
			vec3 H = normalize(L + normalize(ro - pos));
			const float D = std::max(0.0f, dot(N, L));
			const float S = pow(std::max(0.0f, dot(H, N)), 16.0f);
			vec3 color = ambient + (D * base_color + S * light_color * base_color) / distSquared(light_pos, pos);
			//color = vec3(1.0f / (3.0f*z*z*z));
			//color = N;
			color = pow(color, vec3(1.0f/2.2f));
			screen.setPixel(k, color);
		}
		
		screen.draw();
        glfwSwapBuffers(window.getWindow());    	
        glClear(GL_COLOR_BUFFER_BIT);
    	screen.clear();
    	zbuf.clear();
    	
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

