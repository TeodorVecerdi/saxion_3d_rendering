#version 330

uniform sampler2D diffuseTexture;
in vec2 texCoord;
out vec4 fragment_color;

void main() {
	fragment_color = texture(diffuseTexture, texCoord);
}