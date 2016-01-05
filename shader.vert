#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in float radius;

uniform mat4 VP;
uniform mat3 normMat;

smooth out vec3 fragPos;
smooth out vec3 fragNormal;
smooth out vec3 fragColor;

void main(){
	gl_Position = VP * vec4(position, 1);
	gl_PointSize = clamp(radius / gl_Position.w, 1.0f, 100.0f);
	fragPos = position;
	fragNormal = normalize(normMat * normal);
	fragColor = color;
}
