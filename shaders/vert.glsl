#version 410

uniform mat4 MVP;
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vCol;
//layout(location=1) in vec2 vTex;
flat out vec3 color;
//out vec2 texUV;

void main() {
	gl_Position = MVP * vec4(vPos, 1.0);
	color = vCol;
//	texUV = vTex;
}
