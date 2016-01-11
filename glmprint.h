#ifndef GLM_PRINT_H
#define GLM_PRINT_H

#include "glm/glm.hpp"
#include "stdio.h"

inline void print(const glm::vec2& v){
	#ifdef DEBUG
	printf("(%.3f, %.3f)\n", v.x, v.y);
	#endif
}

inline void print(const glm::vec3& v){
	#ifdef DEBUG
	printf("(%.3f, %.3f, %.3f)\n", v.x, v.y, v.z);
	#endif
}

inline void print(const glm::vec4& v){
	#ifdef DEBUG
	printf("(%.3f, %.3f, %.3f, %.3f)\n", v.x, v.y, v.z, v.w);
	#endif
}

inline void print(const glm::mat3& m){
	#ifdef DEBUG
	print(m[0]);
	print(m[1]);
	print(m[2]);
	#endif
}

inline void print(const glm::mat4& m){
	#ifdef DEBUG
	print(m[0]);
	print(m[1]);
	print(m[2]);
	print(m[3]);
	#endif
}

#endif
