#version 330

in vec3 vertex;
//uniform vec2 mousePosition;
//out float distance;

void main() {
	gl_Position = vec4(vertex ,1);
	//distance = length(vertex.xy - mousePosition);
}