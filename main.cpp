#include "myglheaders.h"
#include "octree.h"
#include <iostream>
#include "time.h"
#include <random>
#include "window.h"
#include "input.h"
#include "glprogram.h"
#include "debugmacro.h"

using namespace std;
using namespace glm;

struct Interval3{
	glm::vec3 min, max;
	Interval3(const glm::vec2& u, const glm::vec2& v, const vec2& t)
		: min(u.x, u.y, t.x), max(v.x, v.y, t.y){};
	Interval3(const glm::vec3& l, const glm::vec3& u)
		: min(l), max(u){};
	Interval3& operator + (const Interval3& other){
		min = min + other.min;
		max = max + other.max;
		return *this;
	}
	Interval3& operator - (const Interval3& other){
		min = min = other.max;
		max = max = other.min;
		return *this;
	}
	Interval3& operator * (const Interval3& other){
		float temp[4];
		temp[0] = min.x*other.min.x;
		temp[1] = min.x*other.max.x;
		temp[2] = max.x*other.min.x;
		temp[3] = max.x*other.max.x;
		min.x = std::min(temp[0], std::min(temp[1], std::min(temp[2], temp[3])));
		max.x = std::max(temp[0], std::max(temp[1], std::max(temp[2], temp[3])));
		temp[0] = min.y*other.min.y;
		temp[1] = min.y*other.max.y;
		temp[2] = max.y*other.min.y;
		temp[3] = max.y*other.max.y;
		min.y = std::min(temp[0], std::min(temp[1], std::min(temp[2], temp[3])));
		max.y = std::max(temp[0], std::max(temp[1], std::max(temp[2], temp[3])));
		temp[0] = min.z*other.min.z;
		temp[1] = min.z*other.max.z;
		temp[2] = max.z*other.min.z;
		temp[3] = max.z*other.max.z;
		min.z = std::min(temp[0], std::min(temp[1], std::min(temp[2], temp[3])));
		max.z = std::max(temp[0], std::max(temp[1], std::max(temp[2], temp[3])));
		return *this;
	}
	Interval3& operator / (const Interval3& other){
		Interval3 inv(1.0f / other.max, 1.0f / other.min);
		*this = *this * inv;
		return *this;
	}
	Interval3& opUnion(const Interval3& other){
		min.x = std::min(min.x, other.min.x);
		min.y = std::min(min.y, other.min.y);
		min.z = std::min(min.z, other.min.z);
		max.x = std::max(max.x, other.max.x);
		max.y = std::max(max.y, other.max.y);
		max.z = std::max(max.z, other.max.z);
		return *this;
	}
	Interval3& opIntersect(const Interval3& other){
		min.x = std::max(min.x, other.min.x);
		min.y = std::max(min.y, other.min.y);
		min.z = std::max(min.z, other.min.z);
		max.x = std::min(max.x, other.max.x);
		max.y = std::min(max.y, other.max.y);
		max.z = std::min(max.z, other.max.z);
		return *this;
	}
	Interval3& translate(const glm::vec3& v){
		min += v;
		max += v;
		return *this;
	}
	Interval3& scale(const glm::vec3& v){
		min *= v;
		max *= v;
		return *this;
	}
	Interval3& transform(const glm::mat4& m){
		glm::vec4 v(min.x, min.y, min.z, 1.0f);
		v = m * v;
		min.x = v.x;
		min.y = v.y;
		min.z = v.z;
		v.x = max.x;
		v.y = max.y;
		v.z = max.z;
		v = m * v;
		max.x = v.x;
		max.y = v.y;
		max.z = v.z;
		return *this;
	}
	bool hasZero(){
		return min.x < 0.0f && max.x > 0.0f && min.y < 0.0f && max.y > 0.0f && min.z < 0.0f && max.z > 0.0f;
	}
	float radius(){
		return 0.5f * glm::distance(min, max);
	}
	float volume(){
		glm::vec3 dim = max - min;
		return dim.x * dim.y * dim.z;
	}
};

Interval3 getSphereInterval(vec3& center, float radius){
	return Interval3(center - radius, center + radius);
}

float beamcast(vec2& u, vec2& v, vec2& t, float e){
	
	return 0.0f;
}

int main(){
	Window window(1600, 900, 3, 3, "Octree Testing");
	Input input(window.getWindow());
	GLProgram prog("shader.vert", "shader.frag");
	Camera camera;
	camera.setEye({0.0f, 0.0f, 3.0f});
	
	prog.bind();
	prog.setUniform("light_pos", camera.getEye());
	prog.setUniform("ambient", {0.0001f, 0.0001f, 0.0001f});
	prog.setUniform("light_color", {1.0f, 1.0f, 1.0f});
	prog.setUniformFloat("shininess", 64.0f);
	
    PointBuffer pb;
    pb.reserve(1024);
    
    float zbuffer[1600][900];
    for(auto& i : zbuffer)
    	for(auto& j : i)
    		j = 1024.0f;
    
    
    
    for(float theta = 0.0f; theta < 3.14f; theta += 0.005f){
    	for(float phi = 0.0f; phi < 6.28f; phi += 0.005f){
    		float x = cos(theta) * sin(phi);
    		float z = sin(theta) * sin(phi);
    		float y = cos(phi);
				glm::vec3 pos(x, y, z);
				glm::vec3 normal = glm::normalize(pos);
				pb.push_back(Point(pos, normal, normal, 5.0f));
		}
	}
	float dist = glm::distance(pb[0].pos, pb[1].pos)* sqrt(1600*900);
	std::cout << dist << std::endl;
	for(auto& i : pb){
		i.r = dist;
	}
	
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
	PointMesh pointMesh;
	pointMesh.upload(pb);
	pb.clear();
	
    unsigned i = 0;
    double t = glfwGetTime();
    while (!glfwWindowShouldClose(window.getWindow())){
    	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        double dt = glfwGetTime() - t;
        t += dt;
        
        input.poll(dt, camera);
        prog.setUniform("VP", camera.getVP());
        prog.setUniform("eye", camera.getEye());
        if(glfwGetKey(window.getWindow(), GLFW_KEY_F))
			prog.setUniform("light_pos", camera.getEye());
		pointMesh.draw();
		
        glfwSwapBuffers(window.getWindow());
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
