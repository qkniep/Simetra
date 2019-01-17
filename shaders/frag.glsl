#version 410

in vec3 color;
//in vec2 texUV;
out vec4 fragColor;
//uniform sampler2D texSampler;

void main() {
	fragColor = vec4(color, 1.0);
//	fragColor = vec4(texture(texSampler, texUV).rgb, 1.0);
}
