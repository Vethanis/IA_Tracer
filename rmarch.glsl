#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, r32f) uniform image2D dbuf;

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

#define PI 3.14159265
#define TAU (2*PI)
#define PHI (sqrt(5)*0.5 + 0.5)
#define saturate(x) clamp(x, 0, 1)

// Sign function that doesn't return 0
float sgn(float x) {
	return (x<0)?-1:1;
}
vec2 sgn(vec2 v) {
	return vec2((v.x<0)?-1:1, (v.y<0)?-1:1);
}
float square (float x) {
	return x*x;
}
vec2 square (vec2 x) {
	return x*x;
}
vec3 square (vec3 x) {
	return x*x;
}
float lengthSqr(vec3 x) {
	return dot(x, x);
}
// Maximum/minumum elements of a vector
float vmax(vec2 v) {
	return max(v.x, v.y);
}
float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}
float vmax(vec4 v) {
	return max(max(v.x, v.y), max(v.z, v.w));
}
float vmin(vec2 v) {
	return min(v.x, v.y);
}
float vmin(vec3 v) {
	return min(min(v.x, v.y), v.z);
}
float vmin(vec4 v) {
	return min(min(v.x, v.y), min(v.z, v.w));
}
float fSphere(vec3 p, float r) {
	return length(p) - r;
}
// Plane with normal n (n is normalized) at some distance from the origin
float fPlane(vec3 p, vec3 n, float distanceFromOrigin) {
	return dot(p, n) + distanceFromOrigin;
}
float fBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}

float map(vec3 p, int idx){
	vec3 c = csgp[idx].center.xyz;
	const int type = int(csgp[idx].center.w);
	vec3 dim = csgp[idx].dim.xyz;
	const int mat = int(csgp[idx].dim.w);
	//return fBox(p-c, dim);
	if(type == 0){return fSphere(p-c, dim.x);}
	if(type == 1){return fBox(p-c, dim);}
	return far;
}

float trace(vec3 rd, float end, float e, int idx){
	float t = near;
	for(int i = 0; i < 30; i++){
		float d = map(eye + rd*t, idx);
		if(d < e)return t;
		t += d;
		if(t >= end)break;
	}
	return far;
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
	float end = far;
	for(int i = 0; i < 2; i++){
		float lz = trace(rd, end, 0.00001f, i);
		end = min(end, lz);
	}
	if(end < far)
		imageStore(dbuf, pix, vec4(toExp(end)));
}
