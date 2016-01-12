#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "glm/glm.hpp"
#include "camera.h"
#include "ival.h"

struct Frustum{
	// bln, blf, tln, tlf, brn, brf, trn, trf
	glm::vec3 data[8];
	float near, far;
	// uvt is xy range in [-1, 1], z in positive [near, far] clipping plane range
	Frustum(const Camera& c, const ival3& uvt){
		data[0] = c.getPoint(uvt.x.l, uvt.y.l, uvt.z.l);
		data[1] = c.getPoint(uvt.x.l, uvt.y.l, uvt.z.h);
		data[2] = c.getPoint(uvt.x.l, uvt.y.h, uvt.z.l);
		data[3] = c.getPoint(uvt.x.l, uvt.y.h, uvt.z.h);
		data[4] = c.getPoint(uvt.x.h, uvt.y.l, uvt.z.l);
		data[5] = c.getPoint(uvt.x.h, uvt.y.l, uvt.z.h);
		data[6] = c.getPoint(uvt.x.h, uvt.y.h, uvt.z.l);
		data[7] = c.getPoint(uvt.x.h, uvt.y.h, uvt.z.h);
		near = c.getNear(); far = c.getNear();
	}
	// returns min(xyz) and max(xyz) at a given depth t:[near, far]
	// should be 35 flops, 18 conditional moves
	inline ival3 getInterval(float t)const{
		t = (t - near) / (far - near); // map to [0, 1] for lerp
		ival3 i(glm::lerp(data[0], data[1], t), glm::lerp(data[2], data[3], t));
		i = opUnion(i, glm::lerp(data[4], data[5], t));
		i = opUnion(i, glm::lerp(data[6], data[7], t));
		return i;
	}
	// should be 70 flops, 42 conditional moves
	inline ival3 getInterval(ival t)const{
		return opUnion(getInterval(t.l), getInterval(t.h));
	}
};

#endif
