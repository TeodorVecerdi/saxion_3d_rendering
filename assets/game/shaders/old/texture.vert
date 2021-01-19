#version 330

uniform mat4 mvpMatrix;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord;

void main() {
	gl_Position = mvpMatrix * vec4(vertex, 1.0f);
	texCoord = uv;
}