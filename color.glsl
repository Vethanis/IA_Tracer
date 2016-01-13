#version 330 core

uniform vec3 eye;
uniform vec3 tl;
uniform vec3 tr;
uniform vec3 bl;
uniform vec3 br;

uniform vec3 ambient;
uniform vec3 base_color;
uniform vec3 light_color;
uniform vec3 light_pos;

uniform vec2 ddx;
uniform vec2 ddy;

uniform float near;
uniform float far;

uniform sampler2D depth;

out vec4 out_color;

vec3 getPos(vec2 uv, float z){
	vec3 dir = normalize(mix(mix(tl, tr, uv.x), mix(bl, br, uv.x), uv.y));
	return eye + dir * z;
}

void main(){
	float z = texture(depth, gl_FragCoord.xy).r;
	if(z >= far) discard;
	vec3 pos = getPos(gl_FragCoord.xy, z);
	vec3 xv  = getPos(gl_FragCoord.xy+ddx, texture(depth, gl_FragCoord.xy+ddx).r) - pos;
	vec3 yv  = getPos(gl_FragCoord.xy+ddy, texture(depth, gl_FragCoord.xy+ddy).r) - pos;
	vec3 N = normalize(cross(xv, yv));
	vec3 L = normalize(light_pos - pos);
	vec3 H = normalize(L + normalize(eye - pos));
	float D = max(0.0, dot(N, L));
	float S = 0.0f;
	if(D > 0.0f)
		S = pow(max(0.0f, dot(H, N)), 32.0f);
	//vec3 color = ambient + (D * base_color + S * light_color * base_color) / dot(light_pos - pos, light_pos - pos);
	//color = pow(color, vec3(1.0f/2.2f));
	vec3 color = vec3(texture(depth, gl_FragCoord.xy).r) / far;
	//vec3 color = vec3(0.0f, 1.0f, 0.0f);
	//vec3 color = N;
	out_color = vec4(color.rgb, 1.0f);
}
