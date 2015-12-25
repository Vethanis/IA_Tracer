#ifndef VEC2_H
#define VEC2_H

namespace hlm{
	
class vec3;

class vec2{
public:
	float x, y;
	vec2() : x(0.0f), y(0.0f){};
	vec2(const float _x, const float _y) : x(_x), y(_y) {};
	vec2(const vec3& o);
	
};	// vec2
	const vec2 operator+(const vec2& lhs, const vec2& rhs);
	const vec2 operator+(const vec2& v, const float f);
	const vec2 operator+(const float f, const vec2& v);
	const vec2 operator-(const vec2& lhs, const vec2& rhs);
	const vec2 operator-(const vec2& v, const float f);
	const vec2 operator-(const float f, const vec2& v);
	const vec2 operator*(const vec2& lhs, const vec2& rhs);
	const vec2 operator*(const vec2& v, const float f);
	const vec2 operator*(const float f, const vec2& v);
	const vec2 operator/(const vec2& v, const float f);
	const vec2 operator/(const float f, const vec2& v);

};	// hlm

#endif