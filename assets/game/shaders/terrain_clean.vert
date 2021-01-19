#version 450

struct TerrainData {
    sampler2D heightmap;
    sampler2D splatmap;
	float height;
	float normalStepSize;
};

layout (location = 0) in vec3 i_vertex;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv;

layout (location = 0) uniform mat4 projectionMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 modelMatrix;

uniform TerrainData terrainVert;
uniform float time;

out vec2 uv;
out vec3 normal;
out vec3 fragPosition;

//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
float simplex(vec3 v){ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

// Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients
// ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

// /-----------------------------------------------------------------------------------------\
// |                              3D simplex noise taken from                                |
// |           https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83             |
// \-----------------------------------------------------------------------------------------/


float map(float s, float a1, float a2, float b1, float b2) {
    return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

vec4 splatWeight(vec4 splatTex) {
	vec4 weight = vec4(splatTex.xyz, 0);
	weight.w = clamp(1.0 - (weight.x + weight.y + weight.z), 0.0, 1.0);
	return weight;
}

float heightRaw(vec2 position) {
	return textureLod(terrainVert.heightmap, position, 0).r;
}

float height(vec2 position) {
	float baseHeight = heightRaw(position);
	vec4 weights = splatWeight(textureLod(terrainVert.splatmap, position, 0));

	/// WAVE VARIANT 1
	//float waveHeight = 0.15;
	//float s = (sin(time + i_vertex.x*10 + i_vertex.x * time + i_vertex.z*0) + 1.0) / 2.0;
	//float s2 = (sin(time*4 + i_vertex.x*5 + i_vertex.z*20) + 1.0) / 2.0;
	//float s3 = (sin(time*8 + i_vertex.x*5 + i_vertex.z*1) + 1.0) / 2.0;
	//float s4 = (cos(time*16 + i_vertex.x*50) + 1.0) / 2.0;
	//float s5 = (sin(time + 3.141592) + 1.0) / 2.0;
	//return baseHeight + (waveHeight * 0.1 * s5 + waveHeight * 0.6 * s + waveHeight * 0.2 * s2 + waveHeight * 0.07 * s3 + waveHeight * 0.03 * s4) * weights.g;
	
	/// WAVE VARIANT 2
	//float waveHeight = 0.28;
	//float noise1 = 0.6 * (1.0 + simplex(vec3(2 * i_vertex.xz, time * 0.3))) / 2.0;
	//float noise2 = 0.25 * (1.0 + simplex(vec3(8 * i_vertex.xz, time * 0.5))) / 2.0;
	//float noise3 = 0.15 * (1.0 + simplex(vec3(16 * i_vertex.xz, time * 0.47))) / 2.0;
	//return baseHeight + (noise1 + noise2 + noise3) * waveHeight * weights.g * weights.g;

	/// WAVE VARIANT 3
	float waveHeight = 0.4;
	float wave = (sin(time*2 + position.x*20 + position.y*20) + 1.0) / 2.0;
	return baseHeight + waveHeight * wave * weights.g;
}

vec3 calculateNormalA() {
	float hCurrent = height(i_uv);
	float hLeft    = height(i_uv + vec2(-0.01, 0));
	float hRight   = height(i_uv + vec2(0.01, 0));
	float hUp      = height(i_uv + vec2(0, -0.01));
	float hDown    = height(i_uv + vec2(0, 0.01));

	//float diffA = hLeft - hRight;
	//float diffB = hUp - hDown;
	float diffA1 = hLeft - hCurrent;
	float diffA2 = hCurrent - hRight;
	float diffB1 = hDown - hCurrent;
	float diffB2 = hCurrent - hUp;
	float avgA = (diffA1 + diffA2)/2;
	float avgB = (diffB1 + diffB2)/2;
	return normalize( vec3 (avgA, terrainVert.normalStepSize, avgB) );
}

vec3 calculateNormalB() {
	float hLeft    = height(i_uv + vec2(-0.01, 0));
	float hRight   = height(i_uv + vec2(0.01, 0));
	float hUp      = height(i_uv + vec2(0, -0.01));
	float hDown    = height(i_uv + vec2(0, 0.01));
	float diffA = hLeft - hRight;
	float diffB = hUp - hDown;
	return normalize(vec3(diffA, terrainVert.normalStepSize, diffB));
}

void main() {
	float h = height(i_uv);
	vec3 vertex = vec3(i_vertex.x, terrainVert.height * h, i_vertex.z);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0);

	vec3 norm = calculateNormalA();

	uv = i_uv;
	normal = norm;
	//normal = vec3(modelMatrix * vec4(norm, 0.0));
	fragPosition = vec3(modelMatrix * vec4(vertex, 1.0));
}