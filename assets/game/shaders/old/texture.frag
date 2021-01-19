#version 330

uniform sampler2D diffuseTexture;
in vec2 texCoord;

void main() {
	gl_FragColor = texture(diffuseTexture, texCoord);
}