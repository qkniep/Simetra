#version 410

uniform mat4 MVP;
uniform vec3 lightDir = vec3(1, -1, 1);
uniform vec3 lightColor = vec3(1, 1, 1);
//uniform vec3 ambientLightColor = vec3(1, 1, 1);
uniform float ambientStrength = 1;

layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vCol;
layout(location=2) in vec3 vNormal;

flat out vec3 color;


void main() {
	gl_Position = MVP * vec4(vPos, 1.0);

	float diff = max(dot(vNormal, normalize(lightDir)), 0.0);
	vec3 diffuse = (ambientStrength + diff) / (1 + ambientStrength) * lightColor;

	color = diffuse * vCol;
}
