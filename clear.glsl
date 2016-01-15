#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, r32f) uniform image2D dbuf;

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
	imageStore(dbuf, pix, vec4(1.0f));
}
