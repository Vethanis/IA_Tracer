#ifndef VEC3_H
#define VEC3_H

namespace hlm{
// column major
class mat3;
class vec4;

class vec3{
public:
float x, y, z;
	vec3() : x(0.0f), y(0.0f), z(0.0f){}
	vec3(const float s) : x(s), y(s), z(s){}
	vec3(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z){}
	vec3(const vec3& o) : x(o.x), y(o.y), z(o.z){}
	vec3(const vec4& o);
	
	inline vec3& operator =(const vec3& v){x = v.x; y = v.y; z = v.z; return *this;};
	vec3& operator+=(const vec3& v);
	vec3& operator+=(const float s);
	vec3& operator-=(const vec3& v);
	vec3& operator-=(const float s);
	vec3& operator*=(const float s);
	vec3& operator/=(const float s);
	
}; // vec3

	const vec3 operator+(const vec3& lhs, const vec3& rhs);
	const vec3 operator+(const vec3& v, const float f);
	const vec3 operator+(const float f, const vec3& v);
	const vec3 operator-(const vec3& lhs, const vec3& rhs);
	const vec3 operator-(const vec3& v, const float f);
	const vec3 operator-(const float f, const vec3& v);
	const vec3 operator*(const vec3& lhs, const vec3& rhs);
	const vec3 operator*(const vec3& v, const float f);
	const vec3 operator*(const float f, const vec3& v);
	const vec3 operator/(const vec3& v, const float f);
	const vec3 operator/(const float f, const vec3& v);
	const vec3 operator*(const mat3& mat, const vec3& v);

}; // hlm
#endif // define VEC3_H