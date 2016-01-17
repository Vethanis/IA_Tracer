#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

struct CSGParam{
	vec4 center;
	vec4 dim;
};

layout(std430, binding=1) buffer CSGBlock 
{
	CSGParam csgp[];
};

uniform mat4 IVP;
uniform vec3 eye;
uniform float near;
uniform float far;
uniform int num_prims;

vec3 getPos(in vec2 uv, in float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
	vec2 uv = vec2(pix) / vec2(size.x - 1, size.y - 1);
	uv = uv * 2. - 1.;
	
}
