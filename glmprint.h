#ifndef GLM_PRINT_H
#define GLM_PRINT_H

#include "glm/glm.hpp"
#include "stdio.h"

inline void print(const glm::vec2& v){
	printf("(%.3f, %.3f)\n", v.x, v.y);
}

inline void print(const glm::vec3& v){
	printf("(%.3f, %.3f, %.3f)\n", v.x, v.y, v.z);
}

inline void print(const glm::vec4& v){
	printf("(%.3f, %.3f, %.3f, %.3f)\n", v.x, v.y, v.z, v.w);
}

inline void print(const glm::mat3& m){
	print(m[0]);
	print(m[1]);
	print(m[2]);
}

inline void print(const glm::mat4& m){
	print(m[0]);
	print(m[1]);
	print(m[2]);
	print(m[3]);
}

#endif
