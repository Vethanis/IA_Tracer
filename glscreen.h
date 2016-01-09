#ifndef GLSCREEN_H
#define GLSCREEN_H

#include "string.h"
#include "glm/glm.hpp"

class GLScreen{
	unsigned width, height, vao, vbo, tex;
	float cx, cy;
	glm::vec3 *h_frame;
public:
	GLScreen(unsigned w, unsigned h);
	~GLScreen();
	inline void setPixel(unsigned i, const glm::vec3& color){*(h_frame + i) = color;};
	void clear();
	void draw();
};

#endif
