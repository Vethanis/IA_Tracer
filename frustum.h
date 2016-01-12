#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "glm/glm.hpp"
#include "camera.h"
#include "ival.h"
#include <vector>

// uvt is in ndc
inline ival3 getInterval(const Camera& c, const ival3& uvt){
	ival3 i(c.getPoint(uvt.x.l, uvt.y.l, uvt.z.l), c.getPoint(uvt.x.l, uvt.y.l, uvt.z.h));
	i = opUnion(i, c.getPoint(uvt.x.l, uvt.y.h, uvt.z.l));
	i = opUnion(i, c.getPoint(uvt.x.l, uvt.y.h, uvt.z.h));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.l, uvt.z.l));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.l, uvt.z.h));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.h, uvt.z.l));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.h, uvt.z.h));
	return i;
}

// for bisection without mutating input uvt
inline ival3 getInterval(const Camera& c, const ival3& uvt, ival t){
	ival3 i(c.getPoint(uvt.x.l, uvt.y.l, t.l), c.getPoint(uvt.x.l, uvt.y.l, t.h));
	i = opUnion(i, c.getPoint(uvt.x.l, uvt.y.h, t.l));
	i = opUnion(i, c.getPoint(uvt.x.l, uvt.y.h, t.h));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.l, t.l));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.l, t.h));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.h, t.l));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.h, t.h));
	return i;
}

// dont use odd thread count
void getStartingUVs(unsigned threads, std::vector<ival2>& uvs){
	unsigned divx = 0;
	unsigned divy = 0;
	unsigned p = 1;
	while(p < threads){
		if(divy < divx){
			divy++;
			p = p << 1;
		}
		else {
			divx++;
			p = p << 1;
		}
	}
	unsigned xres = 1 << divx;
	unsigned yres = 1 << divy;
	const float dx = 2.0f / xres;
	const float dy = 2.0f / yres;
	//printf("xres: %u, yres: %u, dx: %.3f, dy: %.3f\n", xres, yres, dx, dy);
	for(unsigned i = 0; i < p; i++){
		unsigned r = yres*i / p;
		unsigned c = i % xres;
		float xmin = -1.0f + c * dx;
		float ymin = -1.0f + r * dy;
		uvs.push_back(ival2(xmin, xmin+dx, ymin, ymin+dy));
		//print(uvs.back());
	}
}

#endif
