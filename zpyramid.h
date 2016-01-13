#ifndef ZPYRAMID_H
#define ZPYRAMID_H

#include <vector>
#include <algorithm>
#include "glm/glm.hpp"
#include "ival.h"

#ifdef DEBUG
	#include <iostream>
#endif

class ZPyramid {
	std::vector<std::vector<std::vector<float> > > buffer;
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
	inline float& operator()(int depth, float x, float y){
		int c = (x + 1.0f) * 0.5f * (sizes[depth].x - 1);
		int r = (y + 1.0f) * 0.5f * (sizes[depth].y - 1);
		return buffer[depth][r][c];
	}
	inline int getMaxDepth(){ return max_depth; }
	inline void resize(int width, int height){
		setSizes(width, height);
		buffer.clear();
		for(const auto& i : sizes){
			buffer.push_back(std::vector<std::vector<float> >(i.y, std::vector<float> (i.x, 1.0)));
		}
	}
	inline void clear(float val){
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
	
	inline void paint(int depth, const ival3& uvt){
		const int cmin = (uvt.x.l+1.0f)*0.5f*(sizes[depth].x-1);
		const int cmax = (uvt.x.h+1.0f)*0.5f*(sizes[depth].x-1);
		const int rmin = (uvt.y.l+1.0f)*0.5f*(sizes[depth].y-1);
		const int rmax = (uvt.y.h+1.0f)*0.5f*(sizes[depth].y-1);
		for(int r = rmin; r < rmax; r++){
			for(int c = cmin; c < cmax; c++){
				buffer[depth][r][c] = std::min(buffer[depth][r][c], uvt.z.h);
			}
		}
	}
	inline void blur(){
		for(int r = 1; r < sizes[max_depth].y-1; r++){
			for(int c = 1; c < sizes[max_depth].x-1; c++){
				float avg = 0.0f;
				for(int dr = r-1; dr < r+2; dr++){
					for(int dc = c-1; dc < c+2; dc++){
						avg += buffer[max_depth][dr][dc];
					}
				}
				buffer[max_depth][r][c] = 0.11111111f * avg;
			}
		}
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
