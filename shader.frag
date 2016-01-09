#version 330 core

in vec2 texcoord;

uniform sampler2D image;

out vec4 out_color;

void main(){
	out_color = texture2D(image, texcoord);
}