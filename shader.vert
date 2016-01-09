#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in float radius;

uniform mat4 VP;

smooth out vec3 fragColor;

void main(){
	gl_Position = VP * vec4(position, 1);
	gl_PointSize = radius / gl_Position.w;
	fragColor = color;
}
