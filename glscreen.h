#ifndef GLSCREEN_H
#define GLSCREEN_H

#include "string.h"
#include "ival.h"

class GLScreen{
	unsigned width, height, vao, vbo;
public:
	GLScreen(unsigned w, unsigned h);
	~GLScreen();
	void draw();
};

#endif
