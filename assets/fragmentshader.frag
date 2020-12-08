#version 330

in vec2 uv;

uniform vec2 checkerboardRowsCols = vec2(1, 1); // checkerboard scale

out vec4 sColor;

void main (void) {
    
    float facX = floor(uv.x * checkerboardRowsCols.x); 
    float facY = floor(uv.y * checkerboardRowsCols.y);
    float factor = sign((mod(facX + facY, 2)));
    

    vec4 color = mix(vec4(1),vec4(0,0,0,1),factor);
//    float gray = length(uv);
//    vec4 color = vec4(vec3(gray), 1);
    sColor = color;
}

