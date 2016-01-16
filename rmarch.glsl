#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, rg32f) uniform image2D dbuf;

struct CSGParam{
	vec4 center; // xyz center, w type id
	vec4 dim;	//xyz dimension, w material id
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

float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}
float fSphere(vec3 p, float r) {
	return length(p) - r;
}
float fBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0.0f)));
}

vec2 map(vec3 p, int idx){
	vec3 c = csgp[idx].center.xyz;
	const int type = int(csgp[idx].center.w);
	vec4 dim = csgp[idx].dim;
	if(type == 0){return vec2(fSphere(p-c, dim.x), dim.w);}
	if(type == 1){return vec2(fBox(p-c, dim.xyz), dim.w);}
	return vec2(FAR, -1.0f);
}

vec2 trace(vec3 rd, float end, float e, int idx){
	float t = NEAR;
	for(int i = 0; i < 30; i++){
		vec2 d = map(EYE + rd*t, idx);
		if(d.x < e)return vec2(t, d.y);
		t += d.x;
		if(t >= end)break;
	}
	return vec2(FAR, -1.0f);
}

vec3 getPos(in vec2 uv, in float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

float toExp(float z){
	return (1./z - 1./NEAR) / (1./FAR - 1./NEAR);
}
float toLin(float f){
	return 1.0 / (f * (1./FAR - 1./NEAR) + (1./NEAR));
}

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
	vec2 uv = vec2(pix) / vec2(size.x - 1, size.y - 1);
	uv = uv * 2. - 1.;
	vec3 rd = normalize(getPos(uv, 1.0f) - EYE);
	vec2 lzm = vec2(NEAR, -1.0f);
	for(int i = 0; i < NPRIMS; i++){
		vec2 r = trace(rd, lzm.x, 0.00001f, i);
		if(r.x < lzm.x) lzm = r;
	}
	if(lzm.x < FAR)
		imageStore(dbuf, pix, vec4(toExp(lzm.x), lzm.y, 0.0f, 0.0f));
}
