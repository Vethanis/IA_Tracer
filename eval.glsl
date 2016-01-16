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

layout(std140, binding=2) uniform CamBlock
{
	mat4 IVP;
	vec4 eye;
	vec4 nfp;	// near, far, num_prims
};
#define EYE eye.xyz
#define NEAR nfp.x
#define FAR nfp.y
#define NPRIMS int(nfp.z)

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
