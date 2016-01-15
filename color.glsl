#version 330 core

smooth in vec2 uv;

uniform sampler2D dbuf;

uniform mat4 IVP;

uniform vec3 eye;
uniform vec3 ambient;
uniform vec3 base_color;
uniform vec3 light_color;
uniform vec3 light_pos;

uniform vec2 ddx;
uniform vec2 ddy;

uniform float light_str;

out vec4 out_color;

vec3 getPos(vec2 uv, float z){
	vec4 t = vec4(uv, z, 1.);
	t = IVP * t;
	return vec3(t / t.w);
}

#define DEPTH

void main(){
	vec2 suv = uv * 0.5 + 0.5;
	float z = texture(dbuf, suv).r;
#ifndef DISCARD
	if(z >= 1.) discard;
#endif
#ifdef COLOR
	vec3 pos = getPos(uv.xy, z);
	vec3 xv  = getPos(uv.xy+ddx, texture(dbuf, suv+.5*ddx).r) - pos;
	vec3 yv  = getPos(uv.xy+ddy, texture(dbuf, suv+.5*ddy).r) - pos;
	vec3 N = normalize(cross(xv, yv));
	vec3 L = normalize(light_pos - pos);
	vec3 H = normalize(L + normalize(eye - pos));
	float D = max(0.0, dot(N, L));
	float S = ( D > 0.0f ) ? pow(max(0., dot(H, N)), 32.) : 0.;
	vec3 color = ambient + (D * base_color + S * light_color * base_color) * light_str / dot(light_pos - pos, light_pos - pos);
	out_color = vec4(pow(color, vec3(1.0f/2.2f)), 1.0);
#endif
#ifdef NORMAL
	vec3 pos = getPos(uv.xy, z);
	vec3 xv  = getPos(uv.xy+ddx, texture(dbuf, suv+.5*ddx).r) - pos;
	vec3 yv  = getPos(uv.xy+ddy, texture(dbuf, suv+.5*ddy).r) - pos;
	vec3 N = normalize(cross(xv, yv));
	out_color = vec4(N, 1.0);
#endif
#ifdef DEPTH
	out_color = vec4(vec3(5.0f - pow(5., texture(dbuf, suv).r)), 1.0);
#endif
#ifdef DISCARD
	out_color = vec4(0.0, 1.0, 0.0, 1.0);
	if(z >= 1.) out_color = vec4(1.0, 0.0, 0.0, 1.0);
#endif
}
