#include "mat3.h"
#include "mat4.h"
#include "vec3.h"
#include <algorithm>

namespace hlm{

mat3::mat3(){
	data[0] = 1.0f; data[1] = 0.0f; data[2] = 0.0f;
	data[3] = 0.0f; data[4] = 1.0f; data[5] = 0.0f;
	data[6] = 0.0f; data[7] = 0.0f; data[8] = 1.0f;
}

mat3::mat3(const float s){
	data[0] = s;    data[1] = 0.0f; data[2] = 0.0f;
	data[3] = 0.0f; data[4] = s;    data[5] = 0.0f;
	data[6] = 0.0f; data[7] = 0.0f; data[8] = s;
}

mat3::mat3(float* array){
	std::copy(array, array + 9, &data[0]);
}

mat3::mat3(const mat4& m){
	data[0] = m[0]; data[1] = m[1]; data[2] = m[2];
	data[3] = m[4]; data[4] = m[5]; data[5] = m[6];
	data[6] = m[8]; data[7] = m[9]; data[8] = m[10];
}

mat3& mat3::operator=(const mat3& other){
	std::copy(&other.data[0], &other.data[9], &data[0]);
	return *this;
}

/*
0, 3, 6
1, 4, 7
2, 5, 8
*/

const mat3 mat3::operator*(const mat3& rhs) const{
	mat3 m;
	// data horizontal
	// rhs vertical
	m(0) = data[0] * rhs[0] + data[3] * rhs[1] + data[6] * rhs[2];
	m(1) = data[1] * rhs[0] + data[4] * rhs[1] + data[7] * rhs[2];
	m(2) = data[2] * rhs[0] + data[5] * rhs[1] + data[8] * rhs[2];
	
	m(3) = data[0] * rhs[3] + data[3] * rhs[4] + data[6] * rhs[5];
	m(4) = data[1] * rhs[3] + data[4] * rhs[4] + data[7] * rhs[5];
	m(5) = data[2] * rhs[3] + data[5] * rhs[4] + data[8] * rhs[5];
	
	m(6) = data[0] * rhs[6] + data[3] * rhs[7] + data[6] * rhs[8];
	m(7) = data[1] * rhs[6] + data[4] * rhs[7] + data[7] * rhs[8];
	m(8) = data[2] * rhs[6] + data[5] * rhs[7] + data[8] * rhs[8];
	return m;
}

const mat3 mat3::operator+(const float s)const{
	mat3 m;
	m(0) = s + data[0]; m(1) = s + data[1]; m(2) = s + data[2];
	m(3) = s + data[3]; m(4) = s + data[4]; m(5) = s + data[5];
	m(6) = s + data[6]; m(7) = s + data[7]; m(8) = s + data[8];
	return m;
}

const mat3 mat3::operator+(const mat3& rhs)const{
	mat3 m;
	m(0) = data[0] + rhs[0]; m(1) = data[1] + rhs[1]; m(2) = data[2] + rhs[2];
	m(3) = data[3] + rhs[3]; m(4) = data[4] + rhs[4]; m(5) = data[5] + rhs[5];
	m(6) = data[6] + rhs[6]; m(7) = data[7] + rhs[7]; m(8) = data[8] + rhs[8];
	return m;
}

const mat3 mat3::operator-(const mat3& rhs)const{
	mat3 m;
	m(0) = data[0] - rhs[0]; m(1) = data[1] - rhs[1]; m(2) = data[2] - rhs[2];
	m(3) = data[3] - rhs[3]; m(4) = data[4] - rhs[4]; m(5) = data[5] - rhs[5];
	m(6) = data[6] - rhs[6]; m(7) = data[7] - rhs[7]; m(8) = data[8] - rhs[8];
	return m;
}

const mat3 mat3::operator-(const float s)const{
	mat3 m;
	m(0) = data[0] - s; m(1) = data[1] - s; m(2) = data[2] - s;
	m(3) = data[3] - s; m(4) = data[4] - s; m(5) = data[5] - s;
	m(6) = data[6] - s; m(7) = data[7] - s; m(8) = data[8] - s;
	return m;
}

const mat3 mat3::operator*(const float s)const{
	mat3 m;
	m(0) = s * data[0]; m(1) = s * data[1]; m(2) = s * data[2];
	m(3) = s * data[3]; m(4) = s * data[4]; m(5) = s * data[5];
	m(6) = s * data[6]; m(7) = s * data[7]; m(8) = s * data[8];
	return m;
}

const mat3 mat3::operator/(const float s)const{
	mat3 m;
	m(0) = data[0] / s; m(1) = data[1] / s; m(2) = data[2] / s;
	m(3) = data[3] / s; m(4) = data[4] / s; m(5) = data[5] / s;
	m(6) = data[6] / s; m(7) = data[7] / s; m(8) = data[8] / s;
	return m;
}

};