#version 330

uniform vec3 diffuseColor;

void main() {
    gl_FragColor = vec4(diffuseColor, 1);
}