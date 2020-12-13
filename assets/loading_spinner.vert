#version 330

in vec3 vertex;
in vec2 uv;

out vec2 texCoord;

void main() {
	gl_Position = vec4(vertex, 1);
	gl_PointSize = 10;
	texCoord = uv;
}