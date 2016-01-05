#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in float radius;

smooth out vec3 fragPos;
smooth out vec3 fragNormal;
smooth out vec3 fragColor;

void main(){
	gl_PointSize = radius;
	gl_Position = vec4(position, 1);
	fragPos = position;
	fragNormal = normal;
	fragColor = color;
}
