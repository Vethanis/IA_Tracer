#ifndef HLM_UTIL_H
#define HLM_UTIL_H

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat3.h"
#include "mat4.h"

#include <cmath>
#include <algorithm>

namespace hlm{
	
constexpr float PI = 3.14159265358979f;
constexpr float PI80 = PI / 180.0f;

inline float dot(const vec3& lhs, const vec3& rhs){
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
inline float dot(const vec4& lhs, const vec4& rhs){
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

inline float distance(const vec3& lhs, const vec3& rhs){
	const vec3 v = lhs - rhs;
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float distance(const vec4& lhs, const vec4& rhs){
	const vec4 v = lhs - rhs;
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

inline float length(const vec3& v){
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float length(const vec4& v){
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

inline vec3 abs(const vec3& v){
	return vec3(std::abs(v.x), std::abs(v.y), std::abs(v.z));
}

inline vec4 abs(const vec4& v){
	return vec4(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
}

inline int min(const int a, const int b){
	return a < b ? a : b;
}

inline int max(const int a, const int b){
	return a > b ? a : b;
}

inline float min(const float a, const float b){
	return a < b ? a : b;
}

inline float max(const float a, const float b){
	return a > b ? a : b;
}

inline int clamp(const int low, const int high, const int val){
	return min(high, max(low, val));
}
inline float clamp(const float low, const float high, const float val){
	return min(high, max(low, val));
}
inline float fract(const float f){
	float i;
	return std::modf(f, &i);
}

inline vec3 fract(const vec3& v){
	float i;
	return vec3(std::modf(v.x, &i),std::modf(v.y, &i),std::modf(v.z, &i));
}
inline vec4 fract(const vec4& v){
	float i;
	return vec4(std::modf(v.x, &i),std::modf(v.y, &i),std::modf(v.z, &i),std::modf(v.w, &i));
}

inline vec3 floor(const vec3& v){
	return vec3(std::floor(v.x),std::floor(v.y),std::floor(v.z));
}
inline vec4 floor(const vec4& v){
	return vec4(std::floor(v.x),std::floor(v.y),std::floor(v.z), std::floor(v.w));
}

inline vec3 ceil(const vec3& v){
	return vec3(std::ceil(v.x),std::ceil(v.y),std::ceil(v.z));
}
inline vec4 ceil(const vec4& v){
	return vec4(std::ceil(v.x),std::ceil(v.y),std::ceil(v.z),std::ceil(v.w));
}

inline vec3 round(const vec3& v){
	return vec3(std::round(v.x),std::round(v.y),std::round(v.z));
}
inline vec4 round(const vec4& v){
	return vec4(std::round(v.x),std::round(v.y),std::round(v.z),std::round(v.w));
}

inline vec3 normalize(const vec3& v){
	float len = length(v);
	return vec3(v.x / len, v.y / len, v.z / len);
}
inline vec4 normalize(const vec4& v){
	float len = length(v);
	return vec4(v.x / len, v.y / len, v.z / len, v.w / len);
}

inline vec2 clamp(const float low, const float high, const vec2& v){
	return vec2(clamp(low, high, v.x), clamp(low, high, v.y));
}
inline vec3 clamp(const float low, const float high, const vec3& v){
	return vec3(clamp(low, high, v.x), clamp(low, high, v.y), 
		clamp(low, high, v.z));
}
inline vec4 clamp(const float low, const float high, const vec4& v){
	return vec4(clamp(low, high, v.x), clamp(low, high, v.y), 
		clamp(low, high, v.z), clamp(low, high, v.w));
}

inline float lerp(const float a, const float b, float alpha){
	alpha = clamp(0.0f, 1.0f, alpha);
	return (1.0f - alpha) * a + alpha * b;
}
inline vec2 lerp(const vec2& a, const vec2& b, float alpha){
	alpha = clamp(0.0f, 1.0f, alpha);
	return (1.0f - alpha) * a + alpha * b;
}
inline vec3 lerp(const vec3& a, const vec3& b, float alpha){
	alpha = clamp(0.0f, 1.0f, alpha);
	return (1.0f - alpha) * a + alpha * b;
}
inline vec4 lerp(const vec4& a, const vec4& b, float alpha){
	alpha = clamp(0.0f, 1.0f, alpha);
	return (1.0f - alpha) * a + alpha * b;
}

mat3 inverse(const mat3& in);
mat3 transpose( const mat3& mat);

mat4 inverse(const mat4& in);
mat4 transpose( const mat4& mat);


inline void print(const float f){
	printf("%f\n", f);
}
inline void print(const vec2& v){
	printf("%.3f, %.3f\n", v.x, v.y);
}
void print(const vec3& v);
void print(const vec4& v);
void print(const mat3& mat);
void print(const mat4& mat);

mat4 lookAt(const vec3& eye, const vec3& at, const vec3& up);	// side = m0,1,2, up = m4,5,6, -forward = m8,9,10

inline vec3 getForward(const mat4& mat){
	return vec3(-mat[2], -mat[6], -mat[10]);
}
inline vec3 getRight(const mat4& mat){
	return vec3(mat[0], mat[4], mat[8]);
}
inline vec3 getUp(const mat4& mat){
	return vec3(mat[1], mat[5], mat[9]);
}

mat4 Wmatrix(const float width, const float height);
mat4 Amatrix(const float hwratio = 9.0f / 16.0f, const float fov = 90.0f);
mat4 Nmatrix(double znear, double zfar);

// aspect here is width / height of the window.
// must divide by 4th element of vec4 to normalize your points if using this matrix.
mat4 GLperspective(double fovy, double aspect, double znear, double zfar);

inline float radians(const float degrees){ return degrees * PI80; }

inline vec3 radians(const vec3& v){
	return vec3(radians(v.x), radians(v.y), radians(v.z));
}
inline vec4 radians(const vec4& v){
	return vec4(radians(v.x), radians(v.y), radians(v.z), radians(v.w));
}

inline vec3 cross(const vec3& lhs, const vec3& rhs){
	vec3 v;
	v.x = lhs.y * rhs.z - lhs.z * rhs.y;
	v.y = lhs.z * rhs.x - lhs.x * rhs.z;
	v.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return v;
}

inline vec3 cross(const vec4& lhs, const vec4& rhs){
	vec3 v;
	v.x = lhs.y * rhs.z - lhs.z * rhs.y;
	v.y = lhs.z * rhs.x - lhs.x * rhs.z;
	v.z = lhs.x * rhs.y - lhs.y * rhs.x;
	return v;
}

// convention of direction vector pointing towards normal
// returned vector points away from normal.
inline vec3 reflect(const vec3& d, const vec3& n){
	return vec3(d - 2.0f * dot(d, n) * n);
}

mat4 rotate(const vec4& v);

inline mat4 scale(const vec3& v){
	mat4 m;
	m(0) = v.x;
	m(5) = v.y;
	m(10) = v.z;
	return m;
}
inline mat4 scale(const vec4& v){
	mat4 m;
	m(0) = v.x;
	m(5) = v.y;
	m(10) = v.z;
	return m;
}
inline mat4 translate(const vec3& v){
	mat4 m;
	m(12) = v.x;
	m(13) = v.y;
	m(14) = v.z;
	return m;
}
inline mat4 translate(const vec4& v){
	mat4 m;
	m(12) = v.x;
	m(13) = v.y;
	m(14) = v.z;
	return m;
}

inline float* value_ptr(vec2& v){
	return &v.x;
}
inline float* value_ptr(vec3& v){
	return &v.x;
}
inline float* value_ptr(vec4& v){
	return &v.x;
}
inline float* value_ptr(mat3& m){
	return &m(0);
}
inline float* value_ptr(mat4& m){
	return &m(0);
}

mat4 rotateX(float angle);
mat4 rotateY(float angle);
mat4 rotateZ(float angle);
mat4 rotateEuler(const vec4& angles);

struct TexLerpTri{
	vec2 p, e1, e2;
	float ze1, ze2, z;
	TexLerpTri(const vec2& p0, const vec2& p1, const vec2& p2, const vec3& zs){
		p   = p0/zs.x;
		e1  = p1/zs.y - p;
		e2  = p2/zs.z - p;
		z   = 1.0f/zs.x;
		ze1 = 1.0f/zs.y - z;
		ze2 = 1.0f/zs.z - z;
	}
	inline vec2 texLerp(const float a, const float b){
		return vec2( (p + a*e1 + b*e2) / (z + a*ze1 + b*ze2) );
	}
};

};

#endif // HLM_UTIL_H
