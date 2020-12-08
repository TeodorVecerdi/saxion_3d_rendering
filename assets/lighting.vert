#version 330

in vec3 vertex;
//uniform vec2 mousePosition;
//out float distance;
out vec3 vertexPos;

void main() {
	gl_Position = vec4(vertex * 2,1);
	vertexPos = vertex;
	//distance = length(vertex.xy - mousePosition);
}