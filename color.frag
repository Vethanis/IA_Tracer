#version 330 core

in vec2 texcoord;

uniform mat4 IVP;
uniform vec3 light_pos;
uniform vec3 eye;

uniform vec3 ambient;
uniform vec3 base_color;
uniform vec3 light_color;

uniform float dx;
uniform float dy;

uniform sampler2D depth;

out vec4 out_color;

vec3 getPos(vec3 uvt){
	vec4 v = vec4(uvt, 1.0);
	v = IVP * v;
	v = v / v.w;
	return vec3(v.xyz);
}

void main(){
	float z = gl_FragDepth; //texture2D(depth, gl_FragCoord.xy).r;
	if(z >= 1.0f) discard;
	vec3 pos = getPos(vec3(gl_FragCoord.xy, z));
	vec3 xv = getPos(vec3(gl_FragCoord.x + dx, gl_FragCoord.y, z)) - pos;
	vec3 yv = getPos(vec3(gl_FragCoord.x, gl_FragCoord.y + dy, z)) - pos;
	vec3 N = normalize(cross(xv, yv));
	vec3 L = normalize(light_pos - pos);
	vec3 H = normalize(L + normalize(eye - pos));
	float D = max(0.0, dot(N, L));
	float S = 0.0f;
	if(D > 0.0f)
		S = pow(max(0.0f, dot(H, N)), 32.0f);
	vec3 color = ambient + (D * base_color + S * light_color * base_color) / dot(light_pos - pos, light_pos - pos);
	color = vec3(z);
	//color = N;
	//color = pow(color, vec3(1.0f/2.2f));
	out_color = vec4(color.rgb, 1.0f);
}
