#version 430 core

layout(local_size_x = 8, local_size_y = 8) in;

layout(binding = 0, r32f) uniform image2D dbuf;

uniform mat4 IVP;
uniform vec3 eye;
uniform float near;
uniform float far;

float sphere(vec3 p, vec3 c, float r){
	return length(p - c) - r;
}

float map(vec3 p){
	return sphere(p, vec3(1.0, 0.5, -0.3), 1.0);
}

float trace(vec3 rd, float e){
	float t = near;
	for(int i = 0; i < 60; i++){
		float d = map(eye + rd*t);
		if(d < e)return t;
		t += d;
		if(t >= far)break;
	}
	return 999999.0f;
}

vec3 getPos(in vec2 uv, in float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

float toExp(float z){
	return (1./z - 1./near) / (1./far - 1./near);
}

void main(){
	ivec2 pix = ivec2(gl_GlobalInvocationID.xy);  
	ivec2 size = imageSize(dbuf);
	if (pix.x >= size.x || pix.y >= size.y) return;
	vec2 uv = vec2(pix) / vec2(size.x - 1, size.y - 1);
	uv = uv * 2. - 1.;
	vec3 rd = normalize(getPos(uv, 1.0f) - eye);
	float lz = trace(rd, 0.0001f);
	float ez = toExp(lz);
	if(ez < 1.)
		imageStore(dbuf, pix, vec4(ez));
}
