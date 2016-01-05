#include "point.h"
#include "myglheaders.h"
#include "debugmacro.h"
#include <algorithm>

PointMesh::PointMesh(){
	num_points = 0;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);	// pos
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)(sizeof(float)*3));	// normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)(sizeof(float)*6));	// color
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(sizeof(float)*9));	// radius
	MYGLERRORMACRO
	
}
PointMesh::~PointMesh(){
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	MYGLERRORMACRO
}

void PointMesh::upload(PointBuffer& points){
	num_points = points.size();
	std::sort(points.begin(), points.end());
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * points.size(), &points[0], GL_STATIC_DRAW);
	MYGLERRORMACRO
}

void PointMesh::draw(){
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays(GL_POINTS, 0, num_points);
	MYGLERRORMACRO
}
