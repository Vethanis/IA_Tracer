#ifndef ZPYRAMID_H
#define ZPYRAMID_H

#include <vector>
#include <algorithm>
#include "glm/glm.hpp"

#ifdef DEBUG
	#include <iostream>
#endif

class ZPyramid {
	std::vector<std::vector<std::vector<double> > > buffer;
	std::vector<glm::ivec2> sizes;
	int max_depth;
	inline void setSizes(int width, int height){
		sizes.clear();
		float a = (float)std::min(width, height);
		int i = 0;
		while(a >= 1.0f){
			sizes.push_back({width >> i, height >> i});
			a = a / 2.0f;
			i++;
		}
		std::reverse(std::begin(sizes), std::end(sizes));
		max_depth = sizes.size() - 1;
	}
public:
	ZPyramid(int width, int height){
		resize(width, height);
	}	
	inline double& operator()(int depth, float x, float y){
		glm::ivec2 size = sizes[depth];
		x = (x+1.0f) * 0.5f;
		y = (y+1.0f) * 0.5f;
		int c = x * (size.x - 1);
		int r = y * (size.y - 1);
		return buffer[depth][r][c];
	}
	inline int getMaxDepth(){ return max_depth; }
	inline void resize(int width, int height){
		setSizes(width, height);
		buffer.clear();
		for(const auto& i : sizes){
			buffer.push_back(std::vector<std::vector<double> >(i.y, std::vector<double> (i.x, 1.0)));
		}
	}
	inline void clear(double val){
		for(auto& d : buffer){
			for(auto& r : d){
				for(auto& c : r)
					c = val;
			}
		}
	}
	inline glm::vec3 getPoint(const glm::mat4& M, float x, float y){
		glm::vec4 v(x, y, (*this)(max_depth, x, y), 1.0f);
		v = M * v;
		v = v / v.w;
		return glm::vec3(v);
	}

#ifdef DEBUG
	inline void debug(){
		std::cout << buffer.size() << std::endl;
		for(const auto& d : buffer){
			std::cout << d[0].size() << " " << d.size() << std::endl;
		}
	}
#endif
};

#endif
