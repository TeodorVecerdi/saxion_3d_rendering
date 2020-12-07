#version 330

uniform float checkerboardScale; 
out vec4 fragment_color;
in vec2 texCoord;

float checkerboard(in vec2 uv, in float scale) {
	float cx = floor(uv.x * scale);
	float cy = floor(uv.y * scale);
	float result = mod(cx + cy, 2.0);
	return sign(result);
}

void main() {
	vec4 white = vec4(1);
	vec4 black = vec4(vec3(0), 1);
	vec3 color = mix(vec3(1), vec3(0.1), checkerboard(texCoord, checkerboardScale));
	fragment_color = vec4(color, 1);
}