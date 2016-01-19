#version 430 core

smooth in vec2 uv;

uniform sampler2D dbuf;

layout(std140, binding=2) uniform CamBlock
{
	mat4 IVP;
	vec4 eye;
	vec4 nfp;	// near, far
	ivec4 whnp; // width, height, num_prims
};

#define EYE eye.xyz

uniform vec3 ambient;
uniform vec3 light_color;
uniform vec3 light_pos;

uniform vec2 ddx;
uniform vec2 ddy;

uniform float light_str;

out vec4 out_color;

vec3 getPos(vec2 uv, float z){
	vec4 t = vec4(uv, z, 1.f);
	t = IVP * t;
	return vec3(t / t.w);
}

#define NORMAL

void main(){
	vec2 suv = uv * 0.5f + 0.5f;
	float z = texture(dbuf, suv).r;
#ifndef DISCARD
	if(z >= 1.f) discard;
#endif
#ifdef COLOR
	float mat = texture(dbuf, suv).g;
	vec3 base_color = vec3(0.1f, 0.4f, 0.9f);
	vec3 pos   = getPos(uv.xy, z);
	vec3 xtan  = getPos(uv.xy+ddx, texture(dbuf, suv+.5f*ddx).r) - pos;
	vec3 ytan  = getPos(uv.xy+ddy, texture(dbuf, suv+.5f*ddy).r) - pos;
	vec3 N = normalize(cross(xtan, ytan));
	vec3 L = normalize(light_pos - pos);
	vec3 H = normalize(L + normalize(EYE - pos));
	float D = max(0.0f, dot(N, L));
	float S = ( D > 0.0f ) ? pow(max(0.f, dot(H, N)), 32.f) : 0.f;
	vec3 lvec = light_pos - pos;
	vec3 color = ambient + (D * base_color + S * light_color * base_color) * light_str / dot(lvec, lvec);
	out_color = vec4(pow(color, vec3(1.0f/2.2f)), 1.0f);
#endif
#ifdef NORMAL
	vec3 pos = getPos(uv.xy, z);
	vec3 xv  = getPos(uv.xy+ddx, texture(dbuf, suv+.5f*ddx).r) - pos;
	vec3 yv  = getPos(uv.xy+ddy, texture(dbuf, suv+.5f*ddy).r) - pos;
	vec3 N = 0.5f*normalize(cross(xv, yv)) + 0.5f;
	out_color = vec4(N, 1.0f);
#endif
#ifdef DEPTH
	out_color = vec4(vec3(1.f - texture(dbuf, suv).r), 1.0f);
#endif
#ifdef DISCARD
	out_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	if(z >= 1.) out_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
#endif
}
