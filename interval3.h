#ifndef INTERVAL3_H
#define INTERVAL3_H

#include "glm/glm.hpp"
#include <algorithm>

struct Interval3{
	glm::vec3 min, max;
	Interval3(const glm::vec2& x, const glm::vec2& y, const glm::vec2& z)
		: min(x.x, y.x, z.x), max(x.y, y.y, z.y){};
	Interval3(const glm::vec3& l, const glm::vec3& u)
		: min(l), max(u){};
	Interval3(const Interval3& o) : min(o.min), max(o.max){};
	inline Interval3& operator + (const Interval3& other){
		min = min + other.min;
		max = max + other.max;
		return *this;
	}
	inline Interval3& operator - (const Interval3& other){
		min = min = other.max;
		max = max = other.min;
		return *this;
	}
	inline Interval3& operator * (const Interval3& other){
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
	inline Interval3& operator / (const Interval3& other){
		Interval3 inv(1.0f / other.max, 1.0f / other.min);
		*this = *this * inv;
		return *this;
	}
	inline void opUnion(const Interval3& other){
		min.x = std::min(min.x, other.min.x);
		min.y = std::min(min.y, other.min.y);
		min.z = std::min(min.z, other.min.z);
		max.x = std::max(max.x, other.max.x);
		max.y = std::max(max.y, other.max.y);
		max.z = std::max(max.z, other.max.z);
	}
	inline void opIntersect(const Interval3& other){
		min.x = std::max(min.x, other.min.x);
		min.y = std::max(min.y, other.min.y);
		min.z = std::max(min.z, other.min.z);
		max.x = std::min(max.x, other.max.x);
		max.y = std::min(max.y, other.max.y);
		max.z = std::min(max.z, other.max.z);
	}
	inline void translate(const glm::vec3& v){
		min += v;
		max += v;
	}
	inline void scale(const glm::vec3& v){
		min *= v;
		max *= v;
	}
	inline void transform(const glm::mat4& m){
		glm::vec4 v(min.x, min.y, min.z, 1.0f);
		v = m * v;
		v = v / v.w;
		min = glm::vec3(v);
		v.x = max.x;
		v.y = max.y;
		v.z = max.z;
		v.w = 1.0f;
		v = m * v;
		v = v / v.w;
		max = glm::vec3(v);
	}
	inline float volume(){
		glm::vec3 dim = max - min;
		return dim.x * dim.y * dim.z;
	}
};

inline glm::vec2 IASphere(const Interval3& p, const glm::vec3& c, const float r){
	const float common = glm::dot(c,c) + r*r;
	const glm::vec2 v(glm::dot(p.min, p.min) - common, glm::dot(p.max, p.max) - common);
	return glm::vec2(std::min(v.x, v.y), std::max(v.x, v.y));
}

#endif
