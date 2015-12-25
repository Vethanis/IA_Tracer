#include "vec4.h"
#include "vec3.h"
#include "mat4.h"

namespace hlm{

vec4::vec4(const vec3& o) : x(o.x), y(o.y), z(o.z), w(0.0f){};

vec4& vec4::operator=(const vec4& other){
	x = other.x; y = other.y; z = other.z; w = other.w;
	return *this;
}

const vec4 operator+(const vec4& lhs, const vec4& rhs){
	return vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

const vec4 operator-(const vec4& lhs, const vec4& rhs){
	return vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

const vec4 operator*(const vec4& lhs, const vec4& rhs){
	return vec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}

vec4& vec4::operator+=(const vec4& other){
	x += other.x; y += other.y; z += other.z; w += other.w;
	return *this;
}

vec4& vec4::operator-=(const vec4& other){
	x -= other.x; y -= other.y; z -= other.z; w -= other.w;
	return *this;
}


const vec4 operator+(const float s, const vec4& v){
	return vec4(s + v.x, s + v.y, s + v.z, s + v.w);
}

const vec4 operator-(const float s, const vec4& v){
	return vec4(s - v.x, s - v.y, s - v.z, s - v.w);
}

const vec4 operator*(const float s, const vec4& v){
	return vec4(s * v.x, s * v.y, s * v.z, s * v.w);
}

const vec4 operator/(const float s, const vec4& v){
	return vec4(s / v.x, s / v.y, s / v.z, s / v.w);
}

const vec4 operator+(const vec4& v, const float s){
	return vec4(v.x + s, v.y + s, v.z + s, v.w + s);
}

const vec4 operator-(const vec4& v, const float s){
	return vec4(v.x - s, v.y - s, v.z - s, v.w - s);
}

const vec4 operator*(const vec4& v, const float s){
	return vec4(v.x * s, v.y * s, v.z * s, v.w * s);
}

const vec4 operator/(const vec4& v, const float s){
	return vec4(v.x / s, v.y / s, v.z / s, v.w / s);
}

vec4& vec4::operator+=(const float s){
	x += s; y += s; z += s; w += s;
	return *this;
}

vec4& vec4::operator-=(const float s){
	x -= s; y -= s; z -= s; w -= s;
	return *this;
}

vec4& vec4::operator*=(const float s){
	x *= s; y *= s; z *= s; w *= s;
	return *this;
}

vec4& vec4::operator/=(const float s){
	x /= s; y /= s; z /= s; w /= s;
	return *this;
}

/*
0,4,8 ,12
1,5,9 ,13
2,6,10,14
3,7,11,15
*/

const vec4 operator*(const mat4& mat, const vec4& v){
	float x = mat[0] * v.x + mat[4] * v.y + mat[8]  * v.z + mat[12] * v.w;
	float y = mat[1] * v.x + mat[5] * v.y + mat[9]  * v.z + mat[13] * v.w;
	float z = mat[2] * v.x + mat[6] * v.y + mat[10] * v.z + mat[14] * v.w;
	float w = mat[3] * v.x + mat[7] * v.y + mat[11] * v.z + mat[15] * v.w;
	return vec4(x, y, z, w);
}

};	// namespace hlm