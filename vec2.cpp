#include "vec2.h"
#include "vec3.h"

namespace hlm{
vec2::vec2(const vec3& o) : x(o.x), y(o.y){};

const vec2 operator+(const vec2& lhs, const vec2& rhs){
	return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
const vec2 operator+(const vec2& v, const float f){
	return vec2(v.x + f, v.y + f);
}
const vec2 operator+(const float f, const vec2& v){
	return vec2(f + v.x, f + v.y);
}
const vec2 operator-(const vec2& lhs, const vec2& rhs){
	return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}
const vec2 operator-(const vec2& v, const float f){
	return vec2(v.x - f, v.y - f);
}
const vec2 operator-(const float f, const vec2& v){
	return vec2(f - v.x, f - v.y);
}
const vec2 operator*(const vec2& lhs, const vec2& rhs){
	return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
}
const vec2 operator*(const vec2& v, const float f){
	return vec2(v.x * f, v.y * f);
}
const vec2 operator*(const float f, const vec2& v){
	return vec2(f * v.x, f * v.y);
}
const vec2 operator/(const vec2& v, const float f){
	return vec2(v.x / f, v.y / f);
}
const vec2 operator/(const float f, const vec2& v){
	return vec2(f / v.x, f / v.y);
}

};