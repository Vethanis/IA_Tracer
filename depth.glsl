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
vec2 ipow4(vec2 a){
	return (a.x>=0.0)?vec2(a*a*a*a):(a.y<0.0)?vec2((a*a*a*a).yx):vec2(0.0,max(a.x*a.x*a.x*a.x,a.y*a.y*a.y*a.y));
}
vec2 ipow6(vec2 a){
	return (a.x>=0.0)?vec2(a*a*a*a*a*a):(a.y<0.0)?vec2((a*a*a*a*a*a).yx):vec2(0.0,max(a.x*a.x*a.x*a.x*a.x*a.x,a.y*a.y*a.y*a.y*a.y*a.y));
}
vec2 imin(vec2 a, vec2 b){
	return vec2(min(a.x,b.x),min(a.y,b.y));
}
vec2 imax(vec2 a, vec2 b){
	return vec2(max(a.x,b.x),max(a.y,b.y));
}
vec3 imin(vec3 a, vec3 b){
	return vec3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}
vec3 imax(vec3 a, vec3 b){
	return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}
float imin(vec3 a){
	return min(min(a.x, a.y), a.z);
}
float imax(vec3 a){
	return max(max(a.x, a.y), a.z);
}
vec2 iadd(vec2 a, vec2 b){
	return a + b;
}
vec2 isub(vec2 a, vec2 b){
	return a - b.yx;
}
vec2 ix(vec3 a, vec3 b){
	return vec2(min(a.x, b.x), max(a.x, b.x));
}
vec2 iy(vec3 a, vec3 b){
	return vec2(min(a.y, b.y), max(a.y, b.y));
}
vec2 iz(vec3 a, vec3 b){
	return vec2(min(a.z, b.z), max(a.z, b.z));
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
vec2 iunion(vec2 a, vec2 b){
	return vec2(min(a.x, b.x), max(a.y, b.y));
}
vec2 iintersect(vec2 a, vec2 b){
	return vec2(max(a.x, b.x), min(a.y, b.y));
}
vec2 isphere(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow2(x-c.x) + ipow2(y-c.y) + ipow2(z-c.z) - r*r;
}
vec2 isphere2(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow4(x-c.x) + ipow4(y-c.y) + ipow4(z-c.z) - r*r;
}
vec2 isphere3(vec2 x, vec2 y, vec2 z, vec3 c, float r){
	return ipow6(x-c.x) + ipow6(y-c.y) + ipow6(z-c.z) - r*r;
}
vec2 ibox(vec3 l, vec3 h, vec3 c, vec3 d){
	vec3 a = c - d;
	vec3 b = c + d;
	vec3 la = l - a;
	vec3 lb = l - b;
	vec3 ha = h - a;
	vec3 hb = h - b;
	return vec2(imax(imin(la, lb)), imin(imax(ha, hb)));
	
}
bool contains(vec2 a, float s){
	return a.x <= s && a.y >= s;
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

vec2 widen(vec2 t){
	float c = 0.5*width(t);
	return vec2(t.x - c, t.y + c);
}

vec2 map(vec3 a, vec3 b){
	vec2 c = ix(a, b); vec2 d = iy(a, b); vec2 e = iz(a, b);
	//vec3 l = imin(a, b); vec3 h = imax(a, b);
	return isphere(c, d, e, vec3(0.0f), 1.0f);
	//return ibox(l, h, vec3(0.), vec3(1.));
}

vec3 getPos(in vec2 uv, in float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

vec2 trace(vec2 uv, vec2 t, float e){
	for(int i = 0; i < 60; i++){
		vec2 t0 = vec2(t.x, center(t));
		vec2 F = map(getPos(uv, t0.x), getPos(uv, t0.y));
		if(contains(F, 0.0f)){
			t = t0;
			if(width(t) < e)
				return t;
			continue;
		}
		t0 = vec2(t0.y, t.y);
		F = map(getPos(uv, t0.x), getPos(uv, t0.y));
		if(contains(F, 0.0f)){
			t = t0;
			if(width(t) < e)
				return t;
			continue;
		}
		t = widen(t);
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
	float depth = center(trace(uv, vec2(0., 1.), 0.000001f));
	if(depth < 1.)
		imageStore(dbuf, pix, vec4(depth));
}
