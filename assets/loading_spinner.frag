#version 330

in vec2 texCoord;

uniform float spinnerCount = 15;
uniform float spinnerSize = 0.1;
uniform float minSpinnerSize = 0.01;
uniform float spinnerDistance = 0.6;
uniform float rotationSpeed = 1;
uniform float time;

const float pi = 3.1415926f;

float map(in float s, in float a1, in float a2, in float b1, in float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}
vec2 map(in vec2 s, in float a1, in float a2, in float b1, in float b2){
	return vec2(map(s.x, a1, a2, b1, b2), map(s.y, a1, a2, b1, b2));
}
float radialGradient(in vec2 uv) {
	uv *= vec2(1,-1);
	float angleRaw = atan(uv.x/uv.y);
	float angle = map(angleRaw, -pi/2, pi/2, 0, 0.5);
	if(uv.y > 0) {
		angle = map(angleRaw, -pi/2, pi/2, 0.5, 1);
	}
	return angle;
}
float snap(in float value, in float multiple) {
	float remainder = mod(value, multiple);
	return value - remainder;
}
vec2 toCartesian(in float angle) {
	return vec2(cos(angle), sin(angle));
}
float greater(in float a, in float b) {
	if(a > b) return 1;
	return 0;
}


void main() {
	// move UV to center
	vec2 uv = map(texCoord, 0, 1, -1, 1);
	
	// Calculate radial gradient from UV
	float radial = radialGradient(uv);
	float gradient = radial * 2 * pi;
	float timeOffset = mod(radial + time * rotationSpeed, 1);
	
	// Convert gradient from [0->1] to [0->1->0]
	float rotatedGradient = timeOffset;
	if(timeOffset >= 0.5) rotatedGradient = 1-timeOffset;
	rotatedGradient *= 2;

	// Make gradient snap at <spinnerCount> intervals 
	float spinnerAngle = 2*pi/spinnerCount;
	gradient = snap(gradient, spinnerAngle) + spinnerAngle/2.0;

	// Convert to cartesian and set distance from center
	vec2 cartesian = toCartesian(gradient);
	cartesian *= spinnerDistance;
	
	float dist = distance(cartesian, uv);
	float size = spinnerSize * (clamp(rotatedGradient-minSpinnerSize, 0, 1)) + minSpinnerSize;
	dist = 1-greater(dist, size);
	
	gl_FragColor = vec4(vec3(dist), 1);
}