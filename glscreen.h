#ifndef GLSCREEN_H
#define GLSCREEN_H

#include "string.h"

class GLScreen{
	unsigned vao, vbo;
public:
	GLScreen();
	~GLScreen();
	void draw();
};

#endif
