#ifndef POINT_H
#define POINT_H

#include <vector>
#include "glm/glm.hpp"

struct Point{
    glm::vec3 pos, norm, col;
    float r;
    Point(const glm::vec3& p, const glm::vec3& n, const glm::vec3& c, float _r)
    : pos(p), norm(n), col(c), r(_r){};
    bool operator < (const Point& other){
    	return r < other.r;
    }
};

typedef std::vector<Point> PointBuffer;

struct PointMesh{
	PointBuffer points;
	unsigned vao, vbo;
	PointMesh();
	~PointMesh();
	void draw();
	void upload(PointBuffer& buffer);
};

#endif
