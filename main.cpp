#include "myglheaders.h"
#include "ival.h"
#include <iostream>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"
#include "glscreen.h"
#include "zpyramid.h"
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
	ival d = map(getInterval(c, args.uvt, t));
	cout << "F(B): "; print(d);
	if(!d.contains(0.0f)) return nullptr;
	for(int i = 1; i < 60; i++){
		ival2 ts = split(t);
		d = map(getInterval(c, args.uvt, t));
		cout << "F(B): "; print(d);
		if(d.contains(0.0f)){
			t = ts.x;
			if(t.width() < args.e)
				return new ival(t);
			continue;
		}
		d = map(getInterval(c, args.uvt, t));
		cout << "F(B): "; print(d);
		if(d.contains(0.0f)){
			t = ts.y;
			if(t.width() < args.e)
				return new ival(t);
			continue;
		}
		float dd = t.center();
		t.l += dd;
		t.h += dd*2.0f;
		if(t.h >= args.uvt.z.h) return nullptr;
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

void subdivide(unsigned p, const Camera& cam, ZPyramid& pyr, const ival2& uv, float e){
	int depth = pyr.getDepthFromP(p);
	cout << "Starting Depth: " << depth << endl;
	const int max_depth = pyr.getMaxDepth();
	cout << "Max Depth: " << max_depth << endl;
	vector<SubArgs> stack;
	stack.push_back({{uv.x, uv.y, {0.0f, 1.0f}}, e, depth});
	while(!stack.empty()){
		SubArgs args = stack.back();
		stack.pop_back();
		cout << "Depth: " << args.depth << endl;
		cout << "B0: "; print(args.uvt);
		ival *t = trace(cam, args);
		if(t){
			if(t->h < pyr(args.depth, args.uvt.x.center(), args.uvt.y.center())){
				args.uvt.z = *t;
				cout << "B1: "; print(args.uvt);
				pyr.paint(args.depth, args.uvt);
				if(args.depth <= max_depth)
					split(stack, args);
			}
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
	
	std::vector<ival2> uvs;
	getStartingUVs(threads, uvs);

	const float dx = 2.0f / WIDTH;
	const float dy = 2.0f / HEIGHT;
	Window window(WIDTH, HEIGHT, 3, 3, "IA Beam Casting");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	prog.bind();
	GLScreen screen(WIDTH, HEIGHT);
	ZPyramid pyramid(WIDTH, HEIGHT);
	const int pf = pyramid.getMaxDepth();
	
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	camera.resize(WIDTH, HEIGHT);
	camera.update();
    
	vec3 ambient(0.1f, 0.05f, 0.05f);
    vec3 light_pos(2.f, 2.f, 2.f);
	vec3 light_color(1.0f);
	vec3 base_color(0.5f, 0.1f, 0.01f);
	
	
	//subdivide(threads, camera, pyramid, uvs[0], 0.1f);
	//return 0;
	
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

		#pragma omp parallel for num_threads(threads) schedule(static, 1)
		for(unsigned k = 0; k < threads; k++){
			subdivide(threads, camera, pyramid, uvs[k], 0.1f);
		}
		
		#pragma omp parallel for num_threads(threads) schedule(dynamic, 12)
		for(int k = 0; k < WIDTH*HEIGHT; k++){
			const int c = clamp(k % WIDTH, 1, WIDTH-2);
			const int r = clamp(k / HEIGHT, 1, HEIGHT-2);
			const float x = c*dx - 1.0f;
			const float y = r*dy - 1.0f;
			const float z = pyramid(pf, x, y);
			if(z == 1.0f) continue;
			vec3 pos = camera.getPoint(x, y, z);
			vec3 xv = camera.getPoint(x+dx, y, pyramid(pf, x+dx, y)) - pos;
			vec3 yv = camera.getPoint(x, y+dy, pyramid(pf, x, y+dy)) - pos;
			vec3 N = normalize(cross(xv, yv));
			vec3 L = normalize(light_pos - pos);
			vec3 H = normalize(L + normalize(ro - pos));
			const float D = std::max(0.0f, dot(N, L));
			const float S = pow(std::max(0.0f, dot(H, N)), 16.0f);
			vec3 color = ambient + (D * base_color + S * light_color * base_color) / distSquared(light_pos, pos);
			color = pow(color, vec3(1.0f/2.2f));
			screen.setPixel(k, color);
		}
		
		screen.draw();
        glfwSwapBuffers(window.getWindow());    	
        glClear(GL_COLOR_BUFFER_BIT);
    	screen.clear();
    	pyramid.clear(1.0f);
    	
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

