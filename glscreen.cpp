#include "glscreen.h"
#include "myglheaders.h"
#include "debugmacro.h"

#define FRAME_BYTES sizeof(float) * width * height

GLScreen::GLScreen(unsigned w, unsigned h) : width(w), height(h){
	cx = width * 0.5f;
	cy = height * 0.5f;
    h_frame = (float*)malloc(FRAME_BYTES);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    static const GLfloat coords[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f,
        -1.0f, -1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords[0]) * 12, coords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
	//glGenTextures(1, &tex);
	//glBindTexture(GL_TEXTURE_2D, tex);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glBindTexture(GL_TEXTURE_2D, 0);
	MYGLERRORMACRO
}

GLScreen::~GLScreen(){
	//glDeleteTextures(1, &tex);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	free(h_frame);
	MYGLERRORMACRO
}

void GLScreen::draw(){
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBindTexture(GL_TEXTURE_2D, tex);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_R, GL_FLOAT, h_frame);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    MYGLERRORMACRO
}

