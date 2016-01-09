#ifndef INTERVAL3_H
#define INTERVAL3_H

#include "glm/glm.hpp"
#include <algorithm>

struct Interval{
	float min, max;
	Interval(float lo, float hi) : min(lo), max(hi){};
	Interval(const Interval& other) : min(other.min), max(other.max){};
	inline Interval& operator = (const Interval& other){
		min = other.min; max = other.max; return *this;}
	inline Interval& operator + (const Interval& other){
		min += other.min; max += other.max; return *this;}
	inline Interval& operator - (const Interval& other){
		min -= other.max; max -= other.min; return *this;}
	inline Interval& operator * (const Interval& other){
		float temp[4] = {	min*other.min, 
							min*other.max, 
							max*other.min, 
							max*other.max 
						};
		min = std::min(std::min(std::min(temp[0], temp[1]), temp[2]), temp[3]);
		max = std::max(std::max(std::max(temp[0], temp[1]), temp[2]), temp[3]);
		return *this;
	}
	inline Interval& operator / (const Interval& other){
		Interval inv(1.0f / other.max, 1.0f / other.min);
		*this = *this * inv;
		return *this;
	}
	inline void opUnion(const Interval& other){
		min = std::min(min, other.min);
		max = std::min(max, other.max);
	}
	inline void opIntersect(const Interval& other){
		min = std::max(min, other.min);
		max = std::min(max, other.max);
	}
	inline float width()const{ return max - min; }
	inline float center()const{ return 0.5f * (max + min);}
};

struct Interval3{
	Interval x, y, z;
	Interval3(const Interval& X, const Interval& Y, const Interval& Z)
		: x(X), y(Y), z(Z){};
	Interval3(const glm::vec3& l, const glm::vec3& u)
		: x(l.x, u.x), y(l.y, u.y), z(l.z, u.z){};
	Interval3(const Interval3& o) : x(o.x), y(o.y), z(o.z){};
	inline Interval3& operator = (const Interval3& other){
		x = other.x; y = other.x; z = other.z; return *this;
	}
	inline Interval3& operator + (const Interval3& other){
		x = x + other.x;
		y = y + other.y;
		z = z + other.z;
		return *this;
	}
	inline Interval3& operator - (const Interval3& other){
		x = x - other.x;
		y = y - other.y;
		z = z - other.z;
		return *this;
	}
	inline Interval3& operator * (const Interval3& other){
		x = x * other.x;
		y = y * other.y;
		z = z * other.z;
		return *this;
	}
	inline Interval3& operator / (const Interval3& other){
		x = x / other.x;
		y = y / other.y;
		z = z / other.z;
		return *this;
	}
	inline void opUnion(const Interval3& other){
		x.opUnion(other.x);
		y.opUnion(other.y);
		z.opUnion(other.z);
	}
	inline void opIntersect(const Interval3& other){
		x.opIntersect(other.x);
		y.opIntersect(other.y);
		z.opIntersect(other.z);
	}
	inline void transform(const glm::mat4& m){
		glm::vec4 v(x.min, y.min, z.min, 1.0f);
		v = m * v;
		v = v / v.w;
		x.min = v.x; y.min = v.y; z.min = v.z;
		v.x = x.max; v.y = y.max; v.z = z.max; v.w = 1.0f;
		v = m * v;
		v = v / v.w;
		x.max = v.x; y.max = v.y; z.max = v.z;
	}
	inline glm::vec3 center()const{
		return glm::vec3(x.center(), y.center(), z.center());
	}
};

inline Interval IASphere(Interval3& p, const glm::vec3& c, const float r){
	const float common = glm::dot(c,c) + r*r;
	p.x = p.x * p.x;
	p.y = p.y * p.y;
	p.z = p.z * p.z;
	return Interval(p.x.min + p.y.min + p.z.min - common, p.x.max + p.y.max + p.z.max - common);
}

#endif
