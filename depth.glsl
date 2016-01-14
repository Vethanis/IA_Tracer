#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, r32f) uniform image2D dbuf;

uniform mat4 IVP;

vec2 imul(vec2 a, vec2 b){
	vec4 f = vec4(a.xxyy * b.xyxy);	
	return vec2(
		min(min(f[0],f[1]),min(f[2],f[3])),
		max(max(f[0],f[1]),max(f[2],f[3])));
}
vec2 imul(float a, vec2 b){
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}
vec2 imul(vec2 b, float a){
	vec2 f = vec2(a*b);	
	return vec2(
		min(f[0],f[1]),
		max(f[0],f[1]));
}
vec2 ipow2(vec2 a){	
	return (a.x>=0.0)?vec2(a*a):(a.y<0.0)?vec2((a*a).yx):vec2(0.0,max(a.x*a.x,a.y*a.y));
}
vec2 imin(vec2 a, vec2 b){
	return vec2(min(a.x,b.x),min(a.y,b.y));
}
vec2 imax(vec2 a, vec2 b){
	return vec2(max(a.x,b.x),max(a.y,b.y));
}
vec2 iadd(vec2 a, vec2 b){
	return a + b;
}
vec2 isub(vec2 a, vec2 b){
	return a - b.yx;
}
vec2 iabs(vec2 a){
  if (a.x >= 0.0)
      return a;
  if (a.y <= 0.0) 
      return vec2(-a.y, -a.x);
  return vec2(0, max(-a.x, a.y));
}
vec2 ismoothmin(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0)));
	return isub(imin(a, b), imul(ipow2(e), 0.25/r));
}
vec2 ismoothmax(vec2 a, vec2 b, float r){
	vec2 e = imin(vec2(r), imax(isub(vec2(r), iabs(isub(a, b))), vec2(0.0)));
	return iadd(imax(a, b), imul(ipow2(e), 0.25/r));
}
vec2 isphere(in vec2 x, in vec2 y, in vec2 z, float r){
	return ipow2(x) + ipow2(y) + ipow2(z) - r*r;
}
bool contains(vec2 a, float s){
	return a.x <= s && a.y > s;
}
float width(vec2 a){
	return a.y - a.x;
}
float center(vec2 a){
	return 0.5*(a.x+a.y);
}
bool invalid(vec2 t){
	return (t.x < 0.) || (t.y > 1.);
}

vec2 map(in vec3 a, in vec3 b){
	return ipow2(isphere(
		vec2(min(a.x, b.x), max(a.x, b.x)), 
		vec2(min(a.y, b.y), max(a.y, b.y)), 
		vec2(min(a.z, b.z), max(a.z, b.z)), 
		1.f));
}

vec3 getPos(in vec2 uv, in float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

vec2 trace(vec3 p0, vec3 p1, vec2 t, float e){
	for(int i = 0; i < 60; i++){
		vec2 t0 = vec2(t.x, center(t));
		vec2 F = map(mix(p0, p1, t0.x), mix(p0, p1, t0.y));
		if(contains(F, 0.0f)){
			t = t0;
			if(width(t) < e)
				return t;
			continue;
		}
		t0 = vec2(t0.y, t.y);
		F = map(mix(p0, p1, t0.x), mix(p0, p1, t0.y));
		if(contains(F, 0.0f)){
			t = t0;
			if(width(t) < e)
				return t;
			continue;
		}
		t.x -= width(t)*.2;
		t.y += width(t)*.2;
		if(invalid(t)) break;
	}
	return vec2(0.0f, 10.0f);
}

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
	vec2 uv = vec2(pix) / vec2(size.x - 1, size.y - 1);
	uv = uv * 2. - 1.;
	vec2 t = vec2(0., 1.);
	float depth = center(trace(getPos(uv, t.x), getPos(uv, t.y), t, 0.00001f));
	imageStore(dbuf, pix, vec4(depth));
}
