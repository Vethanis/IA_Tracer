#ifndef VEC4_H
#define VEC4_H

namespace hlm{
// column major
class mat4;
class vec3;

class vec4{
public:
float x, y, z, w;
	vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f){};
	vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w){};
	vec4(float s) : x(s), y(s), z(s), w(s){};
	vec4(const vec4& o) : x(o.x), y(o.y), z(o.z), w(o.w){};
	vec4(const vec3& other);
	
	vec4& operator =(const vec4& other);
	vec4& operator+=(const vec4& other);
	vec4& operator-=(const vec4& other);
	
	vec4& operator+=(const float s);
	vec4& operator-=(const float s);
	vec4& operator*=(const float s);
	vec4& operator/=(const float s);
	
}; // vec4

	const vec4 operator+(const vec4& lhs, const vec4& rhs);
	const vec4 operator-(const vec4& lhs, const vec4& rhs);
	const vec4 operator*(const vec4& lhs, const vec4& rhs);
	
	const vec4 operator+(const float s, const vec4& v);
	const vec4 operator-(const float s, const vec4& v);
	const vec4 operator*(const float s, const vec4& v);
	const vec4 operator/(const float s, const vec4& v);
	
	const vec4 operator+(const vec4& v, const float s);
	const vec4 operator-(const vec4& v, const float s);
	const vec4 operator*(const vec4& v, const float s);
	const vec4 operator/(const vec4& v, const float s);

	const vec4 operator*(const mat4& mat, const vec4& vec);
}; // hlm

#endif // define VEC4_H