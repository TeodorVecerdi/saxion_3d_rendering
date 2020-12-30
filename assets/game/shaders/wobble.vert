#version 330

uniform mat4 mvpMatrix;
uniform float time;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord;

void main() {
	vec3 vert;
	vert.x = vertex.x + sin(vertex.y + time * 5);
	vert.y = vertex.y + sin(vertex.x + time * 5);
	vert.z = vertex.z + sin(vertex.z + time * 5);
	
	gl_Position = mvpMatrix * vec4(vert, 1.0f);
	texCoord = uv;
}