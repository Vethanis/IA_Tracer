#version 330

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 color;
layout(location=3) in float radius;

uniform mat4 VP;

uniform vec3 ambient;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 eye;
uniform float shininess;

smooth out vec3 fragColor;

void main(){
	gl_Position = VP * vec4(position, 1);
	gl_PointSize = radius / gl_Position.w;
	vec3 L = normalize(light_pos - position);
	float D = max(0.0, dot(L, normal));
	vec3 H = normalize(normalize(eye - position) + L);
	float S = pow(max(0.0, dot(H, normal)), shininess);
	float dist2 = dot(light_pos - position, light_pos - position);
	fragColor = ambient + (D * color + S * color * light_color) / dist2;
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0/2.2));
}
