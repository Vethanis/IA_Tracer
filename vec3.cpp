#include "vec3.h"
#include "vec4.h"
#include "mat3.h"

namespace hlm{

vec3::vec3(const vec4& o) : x(o.x), y(o.y), z(o.z){}

vec3& vec3::operator+=(const vec3& v){
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

vec3& vec3::operator+=(const float s){
	x += s;
	y += s;
	z += s;
	return *this;
}

vec3& vec3::operator-=(const vec3& v){
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vec3& vec3::operator-=(const float s){
	x -= s;
	y -= s;
	z -= s;
	return *this;
}

vec3& vec3::operator*=(const float s){
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

vec3& vec3::operator/=(const float s){
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

const vec3 operator*(const vec3& lhs, const vec3& rhs){
	float x = lhs.x * rhs.x;
	float y = lhs.y * rhs.y;
	float z = lhs.z * rhs.z;
	return vec3(x, y, z);
}

const vec3 operator*(const vec3& v, const float f){
	return vec3(v.x * f, v.y * f, v.z * f);
}

const vec3 operator*(const float f, const vec3& v){
	return vec3(v.x * f, v.y * f, v.z * f);
}

/*
0, 3, 6
1, 4, 7
2, 5, 8
*/

const vec3 operator*(const mat3& mat, const vec3& v){
	float x = mat[0] * v.x + mat[3] * v.y + mat[6] * v.z;
	float y = mat[1] * v.x + mat[4] * v.y + mat[7] * v.z;
	float z = mat[2] * v.x + mat[5] * v.y + mat[8] * v.z;
	return vec3(x, y, z);
}

const vec3 operator/(const vec3& v, const float f){
	return vec3(v.x / f, v.y / f, v.z / f);
}

const vec3 operator/(const float f, const vec3& v){
	return vec3(f / v.x, f / v.y, f / v.z);
}

const vec3 operator+(const vec3& lhs, const vec3& rhs){
	return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

const vec3 operator+(const float f, const vec3& v){
	return vec3(f + v.x, f + v.y, f + v.z);
}

const vec3 operator+(const vec3& v, const float f){
	return vec3(v.x + f, v.y + f, v.z + f);
}

const vec3 operator-(const vec3& lhs, const vec3& rhs){
	return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

const vec3 operator-(const vec3& v, const float f){
	return vec3(v.x - f, v.y - f, v.z - f);
}

const vec3 operator-(const float f, const vec3& v){
	return vec3(f - v.x, f - v.y, f - v.z);
}

};