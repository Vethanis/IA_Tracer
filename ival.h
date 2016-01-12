#ifndef IVAL_H
#define IVAL_H

#include "glm/glm.hpp"
#include <algorithm>
#include <vector>
#include "stdio.h"

struct ival{
	float l, h;
	ival() : l(0.0f), h(0.0f){};
	ival(float a, float b) : l(std::min(a, b)), h(std::min(a, b)){};
	ival(const ival& other) : l(other.h), h(other.h){};
	inline ival& operator=(ival other){
		l = other.l; h = other.h; return *this;
	}
	inline ival& operator+(ival other){
		l = l + other.l; h = h + other.h; return *this;
	}
	inline ival& operator-(ival other){
		l = l - other.h; h = h - other.l; return *this;
	}
	inline ival& operator*(ival other){
		float t[4] = {l*other.l, l*other.h, h*other.l, h*other.h};
		l = std::min(std::min(std::min(t[0], t[1]), t[2]), t[3]);
		h = std::max(std::max(std::max(t[0], t[1]), t[2]), t[3]);
		return *this;
	}
	inline ival& operator/(ival other){
		float t[4] = {l/other.l, l/other.h, h/other.l, h/other.h};
		l = std::min(std::min(std::min(t[0], t[1]), t[2]), t[3]);
		h = std::max(std::max(std::max(t[0], t[1]), t[2]), t[3]);
		return *this;
	}
	inline float width()const{ return h - l; }
	inline float center()const{ return 0.5f*(l+h); }
	inline bool contains(float s){ return l <= s && h > s; }
};

inline ival opUnion(ival a, ival b){
	a.l = std::min(a.l, b.l);
	a.h = std::max(a.h, b.h);
	return a;
}
inline ival opInter(ival a, ival b){
	a.l = std::max(a.l, b.l);
	a.h = std::min(a.h, b.h);
	return a;
}
inline ival opUnion(ival a, float b){
	a.l = std::min(a.l, b);
	a.h = std::max(a.h, b);
	return a;
}
inline ival opInter(float a, ival b){
	b.l = std::max(a, b.l);
	b.h = std::min(a, b.h);
	return b;
}
inline ival operator+(ival a, float s){
	a.l += s; a.h += s; return a;
}
inline ival operator+(float s, ival a){
	a.l += s; a.h += s; return a;
}
inline ival operator-(ival a, float s){
	a.l -= s; a.h -= s; return a;
}
inline ival operator-(float s, ival a){
	a.l = s - a.l; a.h = s - a.h; return a;
}
inline ival operator*(ival a, float s){
	return ival(a.l*s, a.h*s);
}
inline ival operator*(float s, ival a){
	return ival(a.l*s, a.h*s);
}
inline ival operator/(ival a, float s){
	return ival(a.l/s, a.h/s);
}
inline ival operator/(float s, ival a){
	return ival(s/a.l, s/a.h);
}
inline ival pow2(ival a){
	return ival(a*a);
}

struct ival2{
	ival x, y;
	ival2(){};
	ival2(float x1, float x2, float y1, float y2) : x(x1, x2), y(y1, y2){};
	ival2(const ival& a, const ival& b) : x(a), y(b){};
	ival2(const glm::vec2& a, const glm::vec2& b) : x(a.x, b.x), y(a.y, b.y){};
	ival2(const ival2& other) : x(other.x), y(other.y){};
	inline ival2& operator=(const ival2& other){
		x = other.x; y = other.y; return *this;
	}
	inline ival2& operator+(const ival2& other){
		x = x + other.x; y = y + other.y; return *this;
	}
	inline ival2& operator-(const ival2& other){
		x = x - other.x; y = y - other.y; return *this;
	}
	inline ival2& operator*(const ival2& other){
		x = x * other.x; y = y * other.y; return *this;
	}
	inline ival2& operator/(const ival2& other){
		x = x / other.x; y = y / other.y; return *this;
	}
	inline glm::vec2 widths()const{ return glm::vec2(x.width(), y.width());}
	inline float area()const{ return x.width() * y.width(); }
	inline glm::vec2 center()const{return glm::vec2(x.center(), y.center());}
};

inline ival2 operator+(const ival2& a, float s){
	return ival2(a.x+s, a.y+s);
}
inline ival2 operator+(float s, const ival2& a){
	return ival2(a.x+s, a.y+s);
}
inline ival2 operator-(const ival2& a, float s){
	return ival2(a.x-s, a.y-s);
}
inline ival2 operator-(float s, const ival2& a){
	return ival2(s-a.x, s-a.y);
}
inline ival2 operator*(const ival2& a, float s){
	return ival2(a.x*s, a.y*s);
}
inline ival2 operator*(float s, const ival2& a){
	return ival2(a.x*s, a.y*s);
}
inline ival2 operator/(const ival2& a, float s){
	return ival2(a.x/s, a.y/s);
}
inline ival2 operator/(float s, const ival2& a){
	return ival2(s/a.x, s/a.y);
}
inline ival2 operator+(const ival2& a, const glm::vec2& b){
	return ival2(a.x+b.x, a.y+b.y);
}
inline ival2 operator+(const glm::vec2& a, const ival2& b){
	return ival2(a.x+b.x, a.y+b.y);
}
inline ival2 operator-(const ival2& a, const glm::vec2& b){
	return ival2(a.x-b.x, a.y-b.y);
}
inline ival2 operator-(const glm::vec2& a, const ival2& b){
	return ival2(a.x-b.x, a.y-b.y);
}
inline ival2 operator*(const ival2& a, const glm::vec2& b){
	return ival2(a.x*b.x, a.y*b.y);
}
inline ival2 operator*(const glm::vec2& a, const ival2& b){
	return ival2(a.x*b.x, a.y*b.y);
}
inline ival2 operator/(const ival2& a, const glm::vec2& b){
	return ival2(a.x/b.x, a.y/b.y);
}
inline ival2 operator/(const glm::vec2& a, const ival2& b){
	return ival2(a.x/b.x, a.y/b.y);
}
inline ival2 opUnion(const ival2& a, const ival2& b){
	return ival2(opUnion(a.x, b.x), opUnion(a.y, b.y));
}
inline ival2 opInter(const ival2& a, const ival2& b){
	return ival2(opInter(a.x, b.x), opInter(a.y, b.y));
}
inline ival2 opUnion(const ival2& a, const glm::vec2& b){
	return ival2(opUnion(a.x, b.x), opUnion(a.y, b.y));
}
inline ival2 opInter(const glm::vec2& a, const ival2& b){
	return ival2(opInter(a.x, b.x), opInter(a.y, b.y));
}
inline ival2 pow2(ival2 a){
	return a*a;
}
inline ival reduce(ival2 a){
	return a.x + a.y;
}

struct ival3{
	ival x, y, z;
	ival3(){};
	ival3(float x1, float x2, float y1, float y2, float z1, float z2) 
		: x(x1, x2), y(y1, y2), z(z1, z2){};
	ival3(const glm::vec3& a, const glm::vec3& b) : x(a.x, b.x), y(a.y, b.y), z(a.z, b.z){};
	ival3(const ival& a, const ival& b, const ival& c) : x(a), y(b), z(c){};
	ival3(const ival3& other) : x(other.x), y(other.y), z(other.z){};
	inline ival3& operator=(const ival3& other){
		x = other.x; y = other.y; z = other.z; return *this;
	}
	inline ival3& operator+(const ival3& other){
		x = x + other.x; y = y + other.y; z = z + other.z; return *this;
	}
	inline ival3& operator-(const ival3& other){
		x = x - other.x; y = y - other.y; z = z - other.z; return *this;
	}
	inline ival3& operator*(const ival3& other){
		x = x * other.x; y = y * other.y; z = z * other.z; return *this;
	}
	inline ival3& operator/(const ival3& other){
		x = x / other.x; y = y / other.y; z = z / other.z; return *this;
	}
	inline float volume()const{ return x.width() * y.width() * z.width(); }
	inline glm::vec3 center()const{ return glm::vec3(x.center(), y.center(), z.center()); }
	inline glm::vec3 widths()const{ return glm::vec3(x.width(), y.width(), z.width()); }
};

inline ival3 operator+(const ival3& a, float s){
	return ival3(a.x+s, a.y+s, a.z+s);
}
inline ival3 operator+(float s, const ival3& a){
	return ival3(a.x+s, a.y+s, a.z+s);
}
inline ival3 operator-(const ival3& a, float s){
	return ival3(a.x-s, a.y-s, a.z-s);
}
inline ival3 operator-(float s, const ival3& a){
	return ival3(s-a.x, s-a.y, s-a.z);
}
inline ival3 operator*(const ival3& a, float s){
	return ival3(a.x*s, a.y*s, a.z*s);
}
inline ival3 operator*(float s, const ival3& a){
	return ival3(s*a.x, s*a.y, s*a.z);
}
inline ival3 operator/(const ival3& a, float s){
	return ival3(a.x/s, a.y/s, a.z/s);
}
inline ival3 operator/(float s, const ival3& a){
	return ival3(s/a.x, s/a.y, s/a.z);
}
inline ival3 operator+(const ival3& a, const glm::vec3& b){
	return ival3(a.x+b.x, a.y+b.y, a.z+b.z);
}
inline ival3 operator+(const glm::vec3& a, const ival3& b){
	return ival3(a.x+b.x, a.y+b.y, a.z+b.z);
}
inline ival3 operator-(const ival3& a, const glm::vec3& b){
	return ival3(a.x-b.x, a.y-b.y, a.z-b.z);
}
inline ival3 operator-(const glm::vec3& a, const ival3& b){
	return ival3(a.x-b.x, a.y-b.y, a.z-b.z);
}
inline ival3 operator*(const ival3& a, const glm::vec3& b){
	return ival3(a.x*b.x, a.y*b.y, a.z*b.z);
}
inline ival3 operator*(const glm::vec3& a, const ival3& b){
	return ival3(a.x*b.x, a.y*b.y, a.z*b.z);
}
inline ival3 operator/(const ival3& a, const glm::vec3& b){
	return ival3(a.x/b.x, a.y/b.y, a.z/b.z);
}
inline ival3 operator/(const glm::vec3& a, const ival3& b){
	return ival3(a.x/b.x, a.y/b.y, a.z/b.z);
}
inline ival3 opUnion(const ival3& a, const ival3& b){
	return ival3(opUnion(a.x, b.x), opUnion(a.y, b.y), opUnion(a.z, b.z));
}
inline ival3 opInter(const ival3& a, const ival3& b){
	return ival3(opInter(a.x, b.x), opInter(a.y, b.y), opInter(a.z, b.z));
}
inline ival3 opUnion(const ival3& a, const glm::vec3& b){
	return ival3(opUnion(a.x, b.x), opUnion(a.y, b.y), opUnion(a.z, b.z));
}
inline ival3 opInter(const glm::vec3& a, const ival3& b){
	return ival3(opInter(a.x, b.x), opInter(a.y, b.y), opInter(a.z, b.z));
}
inline ival3 pow2(ival3 a){
	return a*a;
}
inline ival reduce(ival3 a){
	return a.x + a.y + a.z;
}

inline ival isphere(const ival3& p, const glm::vec3& c, float r){
	return reduce(pow2(p-c)) - r*r;
}
inline ival isphere2(const ival3& p, const glm::vec3& c, float r){
	return reduce(pow2(pow2(p-c))) - r*r;
}
inline ival isphere4(const ival3& p, const glm::vec3& c, float r){
	return reduce(pow2(pow2(pow2(p-c)))) - r*r;
}
inline ival isphere8(const ival3& p, const glm::vec3& c, float r){
	return reduce(pow2(pow2(pow2(pow2(p-c))))) - r*r;
}
inline ival isphere16(const ival3& p, const glm::vec3& c, float r){
	return reduce(pow2(pow2(pow2(pow2(pow2(p-c)))))) - r*r;
}
inline ival isphere32(const ival3& p, const glm::vec3& c, float r){
	return reduce(pow2(pow2(pow2(pow2(pow2(pow2(p-c))))))) - r*r;
}

inline ival2 split(ival a){
	ival2 r;
	r.x.l = a.l; 
	r.x.h = a.center();
	r.y.l = r.x.h; 
	r.y.h = a.h;
	return r;
}

inline void split(const ival2& a, ival2* ary){
	ival2 u = split(a.x);
	ival2 v = split(a.y);
	ary[0] = ival2(u.x, v.x);
	ary[1] = ival2(u.x, v.y);
	ary[2] = ival2(u.y, v.x);
	ary[3] = ival2(u.y, v.y);
}

inline void split(const ival3& a, ival3* ary){
	ival2 u = split(a.x);
	ival2 v = split(a.y);
	ival2 t = split(a.z);
	ary[0] = ival3(u.x, v.x, t.x);
	ary[1] = ival3(u.x, v.x, t.y);
	ary[2] = ival3(u.x, v.y, t.x);
	ary[3] = ival3(u.x, v.y, t.y);
	ary[4] = ival3(u.y, v.x, t.x);
	ary[5] = ival3(u.y, v.x, t.y);
	ary[6] = ival3(u.y, v.y, t.x);
	ary[7] = ival3(u.y, v.y, t.y);
}

inline void print(ival a){
	printf("[%.3f, %.3f]\n", a.l, a.h);
}
inline void print(const ival2& a){
	printf("([%.3f, %.3f], [%.3f, %.3f])\n", a.x.l, a.x.h, a.y.l, a.y.h); 
}
inline void print(const ival3& a){
	printf("([%.3f, %.3f], [%.3f, %.3f], [%.3f, %.3f])\n", a.x.l, a.x.h, a.y.l, a.y.h, a.z.l, a.z.h); 
}

#endif
