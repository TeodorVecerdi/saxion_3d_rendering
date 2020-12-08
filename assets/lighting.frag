#version 330
//in float distance;

//out vec4 color;
uniform vec2 mousePosition;
uniform vec2 viewportSize;

float map(in float s, in float a1, in float a2, in float b1, in float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

vec2 map(in vec2 s, in float a1, in float a2, in float b1, in float b2){
	return vec2(map(s.x, a1, a2, b1, b2), map(s.y, a1, a2, b1, b2));
}

void main() {
    float ratio = viewportSize.x/viewportSize.y;
	

	vec2 fragPos2 = vec2(ratio * gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
	float mouseX = mousePosition.x * ratio + 0.5/ratio; 
	vec2 mouse = vec2(mouseX, mousePosition.y);
	vec2 fragPosMapped = map(fragPos2, 0, 1, -1, 1);
	

	float dist = (1-distance(mouse, fragPosMapped));

	//float cDist = clamp(dist, -1, 1);
	float gray = dist;
	float size =1;
	gray = gray-1+size;
	gray = map(gray, 0, size, 0, 1);
	
	//color = vec4(vec3(1-gray), 1);
	//gl_FragColor = vec4(fragPos2.x, fragPos2.y, 0, 1);
	gl_FragColor = vec4(gray, gray, gray, 1);
}