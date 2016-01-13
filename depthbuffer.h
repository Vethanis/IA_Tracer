#ifndef DEPTH_BUFFER_H
#define DEPTH_BUFFER_H

#include <algorithm>
#include "ival.h"

class DepthBuffer{
	float* data;
	float width, height;
public:
	DepthBuffer(int w, int h){
		data = new float[w*h];
		width = w;
		height = h;
		for(int i = 0; i < width*height; i++)
			data[i] = 1.0f;
	}
	~DepthBuffer(){
		delete[] data;
	}
	inline float& operator()(float x, float y){
		int c = (x+1.0f)*0.5f*(width-1);
		int r = (y+1.0f)*0.5f*(height-1);
		return data[c*r];
	}
	inline float& operator[](int i){return data[i];}
	inline void paint(const ival3& uvt){
		float z = center(uvt.z);
		int c1 = (uvt.x.l+1.0f)*0.5f*(width-1);
		int c2 = (uvt.x.h+1.0f)*0.5f*(width-1);
		int r1 = (uvt.y.l+1.0f)*0.5f*(height-1);
		int r2 = (uvt.y.h+1.0f)*0.5f*(height-1);
		for(int r = r1; r < r2; r++){
			for(int c = c1; c < c2; c++){
				data[c*r] = std::min(data[c*r], z);
			}
		}
	}
	inline void clear(){
		for(int i = 0; i < width*height; i++)
			data[i] = 1.0f;
	}
};

#endif
