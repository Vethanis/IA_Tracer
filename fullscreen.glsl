#version 330 core

in vec2 vertex;

out vec2 texcoord;

void main(){
	gl_Position = vec4(vertex, 0.0, 1.0);
	
	texcoord = vertex * 0.5 + vec2(0.5, 0.5);
}