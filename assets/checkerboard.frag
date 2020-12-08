#version 330

in vec2 texCoord;

uniform vec2 checkerboardRowsCols = vec2(1, 1); // checkerboard scale

uniform vec2 mousePosition;
uniform vec2 viewportSize;
uniform float lightStrength;
uniform float lightSize;
uniform float ambientLight;

float map(in float s, in float a1, in float a2, in float b1, in float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}
vec2 map(in vec2 s, in float a1, in float a2, in float b1, in float b2){
	return vec2(map(s.x, a1, a2, b1, b2), map(s.y, a1, a2, b1, b2));
}


void main (void) {
    // Checkerboard pattern
    float facX = floor(texCoord.x * checkerboardRowsCols.x); 
    float facY = floor(texCoord.y * checkerboardRowsCols.y);
    float factor = sign((mod(facX + facY, 2)));
    vec4 color = mix(vec4(1),vec4(0,0,0,1),factor);
    
    // Lighting
    float ratio = viewportSize.x/viewportSize.y;
	vec2 pixelPositionRaw = vec2(ratio * gl_FragCoord.x / viewportSize.x, gl_FragCoord.y / viewportSize.y);
	float mouseX = mousePosition.x * ratio + 0.5/ratio; 
	vec2 mousePositionMapped = vec2(mouseX, mousePosition.y);
	vec2 pixelPosition = map(pixelPositionRaw, 0, 1, -1, 1);
	
    // Strength & Size
	float dist = 1-distance(mousePositionMapped, pixelPosition);
	float gray = dist;
	gray = gray-1+lightSize;
	gray = map(gray, 0, lightSize, 0, 1);
    gray = clamp(gray * lightStrength, 0, 1);

    // Blending (Ambient Lighting)
    vec4 finalColor = (ambientLight + (1-ambientLight) * gray) * color;
    finalColor.a = 1;
    
    gl_FragColor = finalColor;
}
