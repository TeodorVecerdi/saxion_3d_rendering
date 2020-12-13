#version 330

in vec3 vertex;
in vec2 uv;

uniform float time;
uniform vec2 offset;
uniform vec2 rotation;
uniform float checkerScale;

out vec2 texCoord;

float map(in float s, in float a1, in float a2, in float b1, in float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}


// Noise functions taken from https://observablehq.com/@riccardoscalco/value-and-gradient-noise-in-glsl
#define MAGIC 43758.5453123

vec2 random2(vec2 st){
    vec2 s = vec2(
      dot(st, vec2(127.1,311.7)),
      dot(st, vec2(269.5,183.3))
    );
    return -1. + 2. * fract(sin(s) * MAGIC);
}

vec4 gradientNoise (vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float f11 = dot(random2(i + vec2(0., 0.)), f - vec2(0., 0.));
    float f12 = dot(random2(i + vec2(0., 1.)), f - vec2(0., 1.));
    float f21 = dot(random2(i + vec2(1., 0.)), f - vec2(1., 0.));
    float f22 = dot(random2(i + vec2(1., 1.)), f - vec2(1., 1.));
    
    return vec4(f11, f12, f21, f22);
}

float interpolate (float t) {
    //return t;
    //return t * t * (3. - 2. * t); // smoothstep
    return t * t * t * (10. + t * (6. * t - 15.)); // smootherstep
}

float noise (vec2 p) {
    vec4 v = gradientNoise(p);
    
    vec2 f = fract(p);
    float t = interpolate(f.x);
    float u = interpolate(f.y);
    
    // linear interpolation 
    return mix(
        mix(v.x, v.z, t),
        mix(v.y, v.w, t), 
        u
    ) * .5 + .5;
}

void main (void) {
    vec3 vert;
    vert.x = vertex.x + cos(vertex.y * 5 + time * 5) * 0.075;
    vert.y = vertex.y + sin(vertex.x * 5 + time * 5) * 0.05;
    //vert.x = vertex.x + noise(vertex.xy * 2 + time * 1) * 0.5;
    //vert.y = vertex.y + noise(vertex.xy * 2 + time * 1) * 0.5;
    vert.z = 0;
    gl_Position = vec4(vert,1) + vec4(offset,0,0);
    
    vec2 pos = uv - vec2(0.5);
    vec2 rotatedPos = vec2(pos.x * rotation.x - pos.y * rotation.y, pos.x * rotation.y + pos.y * rotation.x);
    vec2 scaledPos = rotatedPos * (1 + checkerScale);
    float uvX = map(scaledPos.x, -1, 1, 0, 1);
    float uvY = map(scaledPos.y, -1, 1, 0, 1);

    texCoord = vec2(uvX, uvY) + vec2(0.5);
}

