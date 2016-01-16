#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, rg32f) uniform image2D dbuf;

struct CSGParam{
	vec4 center;
	vec4 dim;
};

layout(std430, binding=1) buffer params 
{
	CSGParam csgp[2];
};

uniform mat4 IVP;
uniform vec3 eye;
uniform float near;
uniform float far;

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
float smin(float a, float b, float k){
	float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

vec2 map(vec3 p, int idx){
	vec3 c = csgp[idx].center.xyz;
	const int type = int(csgp[idx].center.w);
	vec4 dim = csgp[idx].dim;
	if(type == 0){return vec2(fSphere(p-c, dim.x), dim.w);}
	if(type == 1){return vec2(fBox(p-c, dim.xyz), dim.w);}
	return vec2(far, -1.0f);
}

vec2 trace(vec3 rd, float end, float e, int idx){
	float t = near;
	for(int i = 0; i < 30; i++){
		vec2 d = map(eye + rd*t, idx);
		if(d.x < e)return vec2(t, d.y);
		t += d.x;
		if(t >= end)break;
	}
	return vec2(far, -1.0f);
}

vec3 getPos(in vec2 uv, in float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

float toExp(float z){
	return (1./z - 1./near) / (1./far - 1./near);
}
float toLin(float f){
	return 1.0 / (f * (1./far - 1./near) + (1./near));
}

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
	vec2 uv = vec2(pix) / vec2(size.x - 1, size.y - 1);
	uv = uv * 2. - 1.;
	vec3 rd = normalize(getPos(uv, 1.0f) - eye);
	vec2 lzm = vec2(far, -1.0f);
	for(int i = 0; i < 2; i++){
		vec2 r = trace(rd, lzm.x, 0.00001f, i);
		if(r.x < lzm.x) lzm = r;
	}
	if(lzm.x < far)
		imageStore(dbuf, pix, vec4(toExp(lzm.x), lzm.y, 0.0f, 0.0f));
}
