#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <algorithm>
#include "glmprint.h"

#define up_vec glm::vec3(0.0f, 1.0f, 0.0f)

inline glm::vec3 getRight(const glm::mat4& m){
	return glm::vec3(m[0][0], m[1][0], m[2][0]);
}
inline glm::vec3 getUp(const glm::mat4& m){
	return glm::vec3(m[0][1], m[1][1], m[2][1]);
}
inline glm::vec3 getForward(const glm::mat4& m){
	return glm::vec3(-m[0][2], -m[1][2], -m[2][2]);
}

class Camera{
	glm::mat4 P, V, IVP;
	glm::vec3 m_eye, m_at;
	float m_fov, m_whratio, m_near, m_far, m_yaw, m_pitch;
public:
    Camera(float fov=45.0f, float ratio=16.0f/9.0f, float near=0.1f, 
    	float far=5.0f, const glm::vec3& eye=glm::vec3(0.0f), 
    	const glm::vec3& at=glm::vec3(0.0f, 0.0f, -1.0f)) 
    	: m_eye(eye), m_at(at), m_fov(fov), m_whratio(ratio), m_near(near), m_far(far){
		P = glm::perspective(m_fov, m_whratio, m_near, m_far);
		m_yaw = -90.0f;
		m_pitch = 0.0f;
		update();
	}
	inline void update(){
		m_at.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_at.y = sin(glm::radians(m_pitch));
		m_at.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_at += m_eye;
		V = glm::lookAt(m_eye, m_at, up_vec);
		IVP = glm::inverse(P * V);
	}
	inline void resize(int width, int height){
		m_whratio = (double)width / (double)height;
		P = glm::perspective(m_fov, m_whratio, m_near, m_far);
	}
	inline void setFov(float fov){
		m_fov = fov;
		P = glm::perspective(m_fov, m_whratio, m_near, m_far);
	}
	inline const glm::vec3& getEye(){return m_eye;}
	inline const glm::vec3& getAt(){return m_at;}
	inline float getNear()const{return m_near;}
	inline float getFar()const{return m_far;}
	inline float getFov()const{return m_fov;}
	inline void setEye(const glm::vec3& eye){m_eye = eye;}
	// returns world ray from canonical screen coords
	inline glm::vec3 getRay(const glm::vec2& uv)const{
		glm::vec4 temp(uv, 0.0f, 1.0f);
		temp = IVP * temp;
		temp = temp / temp.w;
		return glm::normalize(glm::vec3(temp) - m_eye);
	}
	// returns world coords, x and y are in canonical screen space, z is linear
	inline glm::vec3 getPoint(const glm::vec2& uv, float z)const{
		return m_eye + getRay(uv)*z;
	}
	// input: mat3 of vec2s minmax pairs: 0:U 1:V 2:T
	// output: mat4 of vec3s: 0:bln, 1:blf, 2:trn, 3:trf
	inline glm::mat4 getPoints(const glm::mat3& in)const{
		printf("getPoints input\n"); print(in);
		glm::vec3 bln = getPoint({in[0].x, in[1].x}, in[2].x);
		glm::vec3 blf = getPoint({in[0].x, in[1].x}, in[2].y);
		glm::vec3 trn = getPoint({in[0].y, in[1].y}, in[2].x);
		glm::vec3 trf = getPoint({in[0].y, in[1].y}, in[2].y);
		printf("bln: "); print(bln);
		printf("blf: "); print(blf);
		printf("trn: "); print(trn);
		printf("trf: "); print(trf);
		return glm::mat4(glm::vec4(bln, 1.0f), glm::vec4(blf, 1.0f),
						 glm::vec4(trn, 1.0f), glm::vec4(trf, 1.0f));
	}
	inline void move(const glm::vec3& v){
		m_eye += v.x * getRight(V) + v.y * getUp(V) - v.z * getForward(V);
	}
	void pitch(float amt){
		m_pitch += amt;
		m_pitch = std::max(std::min(89.0f, m_pitch), -89.0f);
	}
	void yaw(float amt){
		m_yaw -= amt;
		m_yaw = fmod(m_yaw, 360.0f);
	}
	inline glm::mat4 getVP()const{
		return P * V;
	}
	inline const glm::mat4& getV(){return V;}
	inline const glm::mat4& getP(){return P;}
	inline const glm::mat4& getIVP(){ return IVP;}
	inline const glm::vec3 getAxis()const{ return normalize(m_at - m_eye);}
};
#endif
