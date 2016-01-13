#version 330 core

in vec2 texcoord;

out float gl_FragDepth;

uniform mat4 IVP;

vec3 lerp(in vec3 l, in vec3 r, float a){
	return (1.0f - a)*l + a*r;
}

vec2 imul(vec2 a, vec2 b){
	vec4 s = vec4(a.x*b.x, a.x*b.y, a.y*b.x, a.y*b.y);
	return vec2(min(min(min(s.x, s.y), s.z), s.w), max(max(max(s.x, s.y), s.z), s.w));
}

vec2 ipow2(vec2 a){
	return imul(a, a);
}

vec2 isphere(in vec2 x, in vec2 y, in vec2 z, float r){
	return ipow2(x) + ipow2(y), + ipow2(z) - r*r;
}

vec2 map(in vec3 a, in vec3 b){
	return isphere(
		vec2(min(a.x, b.x), max(a.x, b.x)), 
		vec2(min(a.y, b.y), max(a.y, b.y)), 
		vec2(min(a.z, b.z), max(a.z, b.z)), 
		1.0f);
}

vec3 getPos(vec2 uv, float t){
	vec4 v = vec4(uv, t, 1.0f);
	v = IVP * v;
	v = v / v.w;
	return vec3(v.xyz);
}

bool contains(vec2 a, float s){
	return a.x < s && a.y > s;
}

float width(vec2 a){
	return a.y - a.x;
}

float center(vec2 a){
	return 0.5*(a.x+a.y);
}

bool invalid(vec2 t){
	return (t.x < 0.0f) || (t.y > 1.0f);
}

vec2 trace(in vec3 p0, in vec3 p1, in vec2 z, in float e){
	vec2 t = z;
	for(int i = 0; i < 60; i++){
		vec2 t0 = vec2(t.x, center(t));
		vec3 start = lerp(p0, p1, t0.x);
		vec3 end = lerp(p0, p1, t0.y);
		vec2 F = map(start, end);
		if(contains(F, 0.0f)){
			t = t0;
			if(width(t) < e)
				return t;
			continue;
		}
		t0 = vec2(t0.y, t.y);
		start = lerp(p0, p1, t0.x);
		end = lerp(p0, p1, t0.y);
		F = map(start, end);
		if(contains(F, 0.0f)){
			t = t0;
			if(width(t) < e)
				return t;
			continue;
		}
		t.x -= e;
		t.y += e;
		if(invalid(t)) break;
	}
	return vec2(0.0f, 10.0f);
}

void main(){
	vec2 uv = gl_FragCoord.xy;
	vec2 t = vec2(0.0f, 1.0f);
	vec3 p0 = getPos(uv, t.x);
	vec3 p1 = getPos(uv, t.y);
	t = trace(p0, p1, t, 0.01f);
	gl_FragDepth = center(t);
}
