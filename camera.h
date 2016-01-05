#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

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
	glm::vec3 m_eye, m_forward;
	float m_fov, m_whratio, m_near, m_far;
public:
    Camera(float fov=90.0f, float ratio=16.0f/9.0f, float near=1.0f, 
    	float far=100.0f, const glm::vec3& eye=glm::vec3(0.0f), 
    	const glm::vec3& at=glm::vec3(0.0f, 0.0f, -1.0f)) 
    	: m_eye(eye), m_fov(fov), m_whratio(ratio), m_near(near), m_far(far){
		m_forward = glm::normalize(at - eye);
		P = glm::perspective(m_fov, m_whratio, m_near, m_far);
		update();
	}
	inline void update(){
		V = glm::lookAt(m_eye, m_forward, up_vec);
		IVP = glm::inverse(P * V);
	}
	inline void setFov(float fov){
		m_fov = fov;
		P = glm::perspective(m_fov, m_whratio, m_near, m_far);
	}
	inline const glm::vec3& getEye(){return m_eye;}
	inline float getFov(){return m_fov;}
	inline void setEye(const glm::vec3& eye){m_eye = eye;}
	inline void lookAt(const glm::vec3& eye, const glm::vec3& at){m_eye = eye; m_forward = glm::normalize(at - eye);}
	inline glm::vec3 getRay(float x, float y){
		glm::vec4 temp(x, y, 0.0f, 1.0f);
		temp = IVP * temp;
		temp = temp / temp.w;
		return glm::normalize(glm::vec3(temp) - m_eye);
	}
	inline void move(const glm::vec3& v){
		m_eye += v.x * getRight(V) + v.y * up_vec + v.z * m_forward;
	}
	void pitch(float amt){
		m_forward += amt * up_vec;
	}
	void yaw(float amt){
		m_forward += amt * getRight(V);
	}
};
#endif
