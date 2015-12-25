#ifndef MAT3_H
#define MAT3_H

namespace hlm{
// column major

class mat4;

class mat3{
	float data[9];
public:
	mat3(float* array);
	mat3(const float s);
	mat3();
	mat3(const mat4& m);
	
	const mat3 operator+(const float s) const;
	const mat3 operator+(const mat3& mat) const;
	const mat3 operator-(const mat3& rhs) const;
	const mat3 operator-(const float s) const;
	const mat3 operator*(const mat3& mat) const;
	const mat3 operator*(const float s) const;
	const mat3 operator/(const float s) const;
	
	inline float& operator()(const int i) { return data[i]; };
	inline const float operator[](const int i) const {return data[i];};
	mat3& operator=(const mat3& other);
};	// mat3

}; // hlm

/*
EXAMPLE OF HOW OPENGL ADDRESSES MATRIX MEMORY:
static void __gluMakeIdentityf(GLfloat m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

SAME AS MAT4:
0, 4, 8,  12,
1, 5, 9,  13,
2, 6, 10, 14,
3, 7, 11, 15

MAT3:
0, 3, 6,
1, 4, 7,
2, 5, 8
*/

#endif //MAT3_H