#ifndef GLSCREEN_H
#define GLSCREEN_H

#include "string.h"
#include "ival.h"

class GLScreen{
	unsigned width, height, vao, vbo, tex;
	float cx, cy;
	float *h_frame;
public:
	GLScreen(unsigned w, unsigned h);
	~GLScreen();
	inline float& operator[](unsigned i){return h_frame[i];}
	inline void paint(const ival3& uvt){
		float z = center(uvt.z);
		int cmin = (uvt.x.l+1.0f)*0.5f*(width-1);
		int cmax = (uvt.x.h+1.0f)*0.5f*(width-1);
		int rmin = (uvt.y.l+1.0f)*0.5f*(height-1);
		int rmax = (uvt.y.h+1.0f)*0.5f*(height-1);
		for(int r = rmin; r < rmax; r++){
			for(int c = cmin; c < cmax; c++)
				h_frame[r*c] = std::min(h_frame[r*c], z);
		}
	}
	inline void clear(){
		for(unsigned i = 0; i < width * height; i++)
			*(h_frame + i) = 1.0f;
	}
	void draw();
};

#endif
