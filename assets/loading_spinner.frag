#version 330

in vec2 texCoord;

float map(in float s, in float a1, in float a2, in float b1, in float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}
vec2 map(in vec2 s, in float a1, in float a2, in float b1, in float b2){
	return vec2(map(s.x, a1, a2, b1, b2), map(s.y, a1, a2, b1, b2));
}

float pi = 3.1415926;

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

vec2 cartesian(in float angle) {
	return vec2(cos(angle), sin(angle));
}

float greaterThan(in float a, in float b) {
	if(a > b) return 1;
	return 0;
}

float lerp(in float a, in float b, in float t) {
    return a + t * (b - a);
}

float ramp(in float t) {
	return t;
}

void main() {
	// inputs
	float spinnerCount = 8;
	float spinnerSize = .2;
	float spinnerDistance = 0.6;
	float time = 0;

	vec2 uv = texCoord;
	uv = map(uv, 0, 1, -1, 1);
	
	float gradient = radialGradient(uv);
	float grad = gradient;
	grad = mod(grad + time, 1);
	float secondGradient = lerp(0, 1, 2*grad);
	if(grad >= 0.5) secondGradient = lerp(0, 1, 1-2 * (grad-0.5));
	gradient *= 2 * pi;
	

	float spinnerAngle = 2*pi/spinnerCount;
	gradient = snap(gradient, spinnerAngle);
	gradient += spinnerAngle/2.0;
	vec2 cart = cartesian(gradient);
	cart *= spinnerDistance;
	float dist = distance(cart, uv);
	dist = greaterThan(dist, spinnerSize);

	gl_FragColor = vec4(vec3(secondGradient), 1);
}