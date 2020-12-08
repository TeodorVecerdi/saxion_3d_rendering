#version 330

in vec3 fColor;
out vec4 sColor;

void main (void) {
    sColor = vec4(fColor,1);
}

