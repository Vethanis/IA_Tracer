#ifndef SUBDIVISION_H
#define SUBDIVISION_H

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
	ival3 i(       c.getPoint(uvt.x.l, uvt.y.l, t.l),
	               c.getPoint(uvt.x.l, uvt.y.l, t.h));
	i = opUnion(i, c.getPoint(uvt.x.l, uvt.y.h, t.l));
	i = opUnion(i, c.getPoint(uvt.x.l, uvt.y.h, t.h));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.l, t.l));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.l, t.h));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.h, t.l));
	i = opUnion(i, c.getPoint(uvt.x.h, uvt.y.h, t.h));
	return i;
}

// dont use odd thread count
void getStartingUVs(float ratio, unsigned threads, std::vector<ival2>& uvs, int& depth_out){
	unsigned divx = 0;
	unsigned divy = 0;
	unsigned p = 1;
	while(p < threads){
		float c_ratio = (float)(1 << divx) / (float)(1 << divy);
		if(c_ratio < ratio)
			divx++;
		else 
			divy++;
		p = p << 1;
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
	depth_out = std::min(divx, divy);
}

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

#endif
