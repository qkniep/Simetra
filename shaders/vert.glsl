#version 410

#define FOG_START 160
#define FOG_END 180

uniform mat4 MVP;
uniform vec3 lightDir = vec3(1, -1, 1);
uniform vec3 lightColor = vec3(1, 1, 1);
//uniform vec3 ambientLightColor = vec3(1, 1, 1);
uniform float ambientStrength = 1;
uniform vec3 bgColor = vec3(0.694, 0.78, 0.925);

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vCol;
layout(location=2) in vec3 vNormal;

flat out vec3 color;


float fogFactorLinear(
		const float dist,
		const float start,
		const float end) {
	return 1.0 - clamp((end - dist) / (end - start), 0.0, 1.0);
}

void main() {
	gl_Position = MVP * vec4(vPos, 1.0);

	float diff = max(dot(vNormal, normalize(lightDir)), 0.0);
	vec3 diffuse = (ambientStrength + diff) / (1 + ambientStrength) * lightColor;

	float fogDistance = gl_Position.z;
	float fogAmount = fogFactorLinear(fogDistance, FOG_START, FOG_END);

	color = diffuse * vCol;

	color = color * (1 - fogAmount) + bgColor * fogAmount;
}
