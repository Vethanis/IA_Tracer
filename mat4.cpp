#include "mat4.h"
#include "vec4.h"
#include <algorithm>

namespace hlm{

mat4::mat4(){
	std::fill(&data[0], &data[16], 0.0f);
	data[0] = 1.0f; data[5] = 1.0f; data[10] = 1.0f; data[15] = 1.0f;
}

mat4::mat4(const float s){
	std::fill(&data[0], &data[16], 0.0f);
	data[0] = s; data[5] = s; data[10] = s; data[15] = s;
}

mat4::mat4(const mat4& other){
	std::copy(&other.data[0], &other.data[16], &data[0]);
}

mat4::mat4(float* array){
	std::copy(array, array + 16, &data[0]);
}

mat4& mat4::operator=(const mat4& other){
	std::copy(&other.data[0], &other.data[16], &data[0]);
	return *this;
}

const mat4 mat4::operator+(const mat4& rhs)const{
	mat4 m;
	m(0)  = data[0]  + rhs[0];  m(1)  = data[1]  + rhs[1];  
	m(2)  = data[2]  + rhs[2];  m(3)  = data[3]  + rhs[3];
	m(4)  = data[4]  + rhs[4];  m(5)  = data[5]  + rhs[5];  
	m(6)  = data[6]  + rhs[6];  m(7)  = data[7]  + rhs[7];
	m(8)  = data[8]  + rhs[8];  m(9)  = data[9]  + rhs[9];  
	m(10) = data[10] + rhs[10]; m(11) = data[11] + rhs[11];
	m(12) = data[12] + rhs[12]; m(13) = data[13] + rhs[13]; 
	m(14) = data[14] + rhs[14]; m(15) = data[15] + rhs[15];
	return m;
}

const mat4 mat4::operator-(const mat4& rhs)const{
	mat4 m;
	m(0)  = data[0]  - rhs[0];  m(1)  = data[1]  - rhs[1];  
	m(2)  = data[2]  - rhs[2];  m(3)  = data[3]  - rhs[3];
	m(4)  = data[4]  - rhs[4];  m(5)  = data[5]  - rhs[5];  
	m(6)  = data[6]  - rhs[6];  m(7)  = data[7]  - rhs[7];
	m(8)  = data[8]  - rhs[8];  m(9)  = data[9]  - rhs[9];  
	m(10) = data[10] - rhs[10]; m(11) = data[11] - rhs[11];
	m(12) = data[12] - rhs[12]; m(13) = data[13] - rhs[13]; 
	m(14) = data[14] - rhs[14]; m(15) = data[15] - rhs[15];
	return m;
}

/*
0,4,8 ,12
1,5,9 ,13
2,6,10,14
3,7,11,15
*/

const mat4 mat4::operator*(const mat4& rhs)const{
	// lhs horizontal
	// rhs vertical
	mat4 m;
	m(0) = data[0]  * rhs[0]  + data[4] * rhs[1]  + data[8]  * rhs[2]  + data[12] * rhs[3];
	m(1) = data[1]  * rhs[0]  + data[5] * rhs[1]  + data[9]  * rhs[2]  + data[13] * rhs[3];
	m(2) = data[2]  * rhs[0]  + data[6] * rhs[1]  + data[10] * rhs[2]  + data[14] * rhs[3];
	m(3) = data[3]  * rhs[0]  + data[7] * rhs[1]  + data[11] * rhs[2]  + data[15] * rhs[3];
	
	m(4) = data[0]  * rhs[4]  + data[4] * rhs[5]  + data[8]  * rhs[6]  + data[12] * rhs[7];
	m(5) = data[1]  * rhs[4]  + data[5] * rhs[5]  + data[9]  * rhs[6]  + data[13] * rhs[7];
	m(6) = data[2]  * rhs[4]  + data[6] * rhs[5]  + data[10] * rhs[6]  + data[14] * rhs[7];
	m(7) = data[3]  * rhs[4]  + data[7] * rhs[5]  + data[11] * rhs[6]  + data[15] * rhs[7];
	
	m(8)  = data[0] * rhs[8]  + data[4] * rhs[9]  + data[8]  * rhs[10] + data[12] * rhs[11];
	m(9)  = data[1] * rhs[8]  + data[5] * rhs[9]  + data[9]  * rhs[10] + data[13] * rhs[11];
	m(10) = data[2] * rhs[8]  + data[6] * rhs[9]  + data[10] * rhs[10] + data[14] * rhs[11];
	m(11) = data[3] * rhs[8]  + data[7] * rhs[9]  + data[11] * rhs[10] + data[15] * rhs[11];
	
	m(12) = data[0] * rhs[12] + data[4] * rhs[13] + data[8]  * rhs[14] + data[12] * rhs[15];
	m(13) = data[1] * rhs[12] + data[5] * rhs[13] + data[9]  * rhs[14] + data[13] * rhs[15];
	m(14) = data[2] * rhs[12] + data[6] * rhs[13] + data[10] * rhs[14] + data[14] * rhs[15];
	m(15) = data[3] * rhs[12] + data[7] * rhs[13] + data[11] * rhs[14] + data[15] * rhs[15];
	return m;
}

const mat4 mat4::operator+(const float s)const{
	mat4 m;
	m(0)  = data[0]  + s; m(1)  = data[1]  + s;  
	m(2)  = data[2]  + s; m(3)  = data[3]  + s;
	m(4)  = data[4]  + s; m(5)  = data[5]  + s;  
	m(6)  = data[6]  + s; m(7)  = data[7]  + s;
	m(8)  = data[8]  + s; m(9)  = data[9]  + s;  
	m(10) = data[10] + s; m(11) = data[11] + s;
	m(12) = data[12] + s; m(13) = data[13] + s; 
	m(14) = data[14] + s; m(15) = data[15] + s;
	return m;
}

const mat4 mat4::operator-(const float s)const{
	mat4 m;
	m(0)  = data[0]  - s; m(1)  = data[1]  - s;  
	m(2)  = data[2]  - s; m(3)  = data[3]  - s;
	m(4)  = data[4]  - s; m(5)  = data[5]  - s;  
	m(6)  = data[6]  - s; m(7)  = data[7]  - s;
	m(8)  = data[8]  - s; m(9)  = data[9]  - s;  
	m(10) = data[10] - s; m(11) = data[11] - s;
	m(12) = data[12] - s; m(13) = data[13] - s; 
	m(14) = data[14] - s; m(15) = data[15] - s;
	return m;	
}

const mat4 mat4::operator*(const float s)const{
	mat4 m;
	m(0)  = data[0]  * s; m(1)  = data[1]  * s;  
	m(2)  = data[2]  * s; m(3)  = data[3]  * s;
	m(4)  = data[4]  * s; m(5)  = data[5]  * s;  
	m(6)  = data[6]  * s; m(7)  = data[7]  * s;
	m(8)  = data[8]  * s; m(9)  = data[9]  * s;  
	m(10) = data[10] * s; m(11) = data[11] * s;
	m(12) = data[12] * s; m(13) = data[13] * s; 
	m(14) = data[14] * s; m(15) = data[15] * s;
	return m;	
}

const mat4 mat4::operator/(const float s)const{
	mat4 m;
	m(0)  = data[0]  / s; m(1)  = data[1]  / s;  
	m(2)  = data[2]  / s; m(3)  = data[3]  / s;
	m(4)  = data[4]  / s; m(5)  = data[5]  / s;  
	m(6)  = data[6]  / s; m(7)  = data[7]  / s;
	m(8)  = data[8]  / s; m(9)  = data[9]  / s;  
	m(10) = data[10] / s; m(11) = data[11] / s;
	m(12) = data[12] / s; m(13) = data[13] / s; 
	m(14) = data[14] / s; m(15) = data[15] / s;
	return m;		
}

}; // hlm
