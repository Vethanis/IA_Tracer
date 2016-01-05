#version 330

smooth in vec3 fragPos;
smooth in vec3 fragNormal;
smooth in vec3 fragColor;

uniform vec3 ambient;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 eye;
uniform float shininess;

out vec4 out_color;

void main(){
	vec3 L = normalize(light_pos - fragPos);
	vec3 V = normalize(eye - fragPos);
	vec3 H = normalize(L + V);
	float S = pow(dot(H, fragNormal), shininess);
	float D = max(0.0, dot(fragNormal, L));
	float dist2 = dot(light_pos - fragPos, light_pos - fragPos);
	vec3 C = ambient + (fragColor * D + fragColor * S * light_color) / dist2;
	out_color = vec4(C, 1.0);
}
