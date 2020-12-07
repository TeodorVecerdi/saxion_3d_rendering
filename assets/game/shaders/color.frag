#version 330

uniform vec3 diffuseColor;
out vec4 fragment_color;

void main() {
    fragment_color = vec4(diffuseColor, 1);
}