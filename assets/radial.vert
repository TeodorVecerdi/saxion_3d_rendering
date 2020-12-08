#version 330


in vec3 vertex;
in vec2 uv;

uniform vec2 offset;
uniform vec2 rotation;
uniform float checkerScale;

out vec2 texCoord;

float map(in float s, in float a1, in float a2, in float b1, in float b2){
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

void main (void) {
    gl_PointSize = 5;
    gl_Position = vec4(vertex,1) + vec4(offset,0,0);

    vec2 pos = vertex.xy;
    vec2 rotatedPos = vec2(pos.x * rotation.x - pos.y * rotation.y, pos.x * rotation.y + pos.y * rotation.x);
    vec2 scaledPos = rotatedPos * (1 + checkerScale);
    float uvX = map(scaledPos.x, -1, 1, 0, 1);
    float uvY = map(scaledPos.y, -1, 1, 0, 1);

    texCoord = vec2(uv.x, uv.y);
}

