#version 450

layout (location = 0) in vec3 i_vertex;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv;

layout (location = 0) uniform mat4 projectionMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 modelMatrix;
layout (location = 3) uniform mat3 normalMatrix;

out vec2 uv;
out vec3 normal;
out vec3 fragPosition;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(i_vertex, 1.0);
	
	uv = i_uv;
	normal = vec3(modelMatrix * vec4(i_normal, 0.0f));
	fragPosition = vec3(modelMatrix * vec4(i_vertex, 1.0));
}