#version 450

#define MAX_LIGHTS 64
#define DIRECTIONAL 1
#define POINT 2
#define SPOTLIGHT 4

struct MaterialData {
    vec4 specular; // rgb=color, a=intensity
    float shininess;
};

struct TerrainData {
    sampler2D splatmap;
    sampler2D heightmap;
    float uvS;//normal step size
    float height;

    sampler2D baseTexture;
    float baseSize;
    sampler2D textureR;
    float sizeR;
    sampler2D waterA;
    float sizeWaterA;
    sampler2D waterB;
    float sizeWaterB;
    sampler2D textureB;
    float sizeB;
};

struct LightData {
    uint type;

    vec3 direction; // used by directional and spot
    vec3 position; // used by point and spot

    vec4 ambient; // rgb=color, a=intensity
    vec4 diffuse; // rgb=color, a=intensity
    vec4 specular; // rgb=color, a=intensity

    vec3 attenuation; // x = constant, y = linear, z = quadratic

    float innerCutOff; // cos(angle)
    float outerCutOff; // cos(angle)
};

in vec2 uv;
in vec3 normal;
in vec3 fragPosition;

out vec4 FragColor;

uniform vec3 eye;
uniform MaterialData material;
uniform TerrainData terrainFrag;
uniform float time;
uniform bool shouldTriplanar;

uniform uint lightCount;
uniform LightData lights[MAX_LIGHTS];

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


vec4 splatWeight(vec4 splatTex) {
	vec4 weight = vec4(splatTex.xyz, 0);
	weight.w = clamp(1.0 - (weight.x + weight.y + weight.z), 0.0, 1.0);
	return weight;
}


vec3 Spotlight(LightData light, vec3 norm, vec3 view, vec3 diffuseSampled, float specularMask) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;

    vec3 lightDir = normalize(light.position - fragPosition);
    vec3 direction = normalize(-light.direction);

    // Spot cut-off
    float theta = dot(lightDir, direction);
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta-light.outerCutOff)/epsilon,0.0,1.0);

    float dist = distance(light.position, fragPosition);
    float distSqr = dist * dist;
    float diffuseAmount = max(dot(norm, lightDir), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse.rgb;

    vec3 r = reflect(-lightDir, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = specularMask * shineFactor * light.specular.rgb;
    
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * distSqr);
    AMBIENT *= attenuation;
    DIFFUSE *= attenuation;
    SPECULAR *= attenuation;

    DIFFUSE *= intensity;
    SPECULAR *= intensity;

    return (
        AMBIENT * diffuseSampled +
        DIFFUSE * diffuseSampled * light.diffuse.a +
        SPECULAR * material.specular.rgb * material.specular.a * light.specular.a
    );
}

vec3 DirectionalLight(LightData light, vec3 norm, vec3 view, vec3 diffuseSampled, float specularMask) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;
    
    vec3 direction = normalize(-light.direction);
    float diffuseAmount = max(dot(norm, direction), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse.rgb;
    
    vec3 r = reflect(-direction, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = specularMask * shineFactor * light.specular.rgb;
    
    return (
        AMBIENT * diffuseSampled +
        DIFFUSE * diffuseSampled * light.diffuse.a +
        SPECULAR * material.specular.rgb * material.specular.a * light.specular.a
     );
}

vec3 PointLight(LightData light, vec3 norm, vec3 view, vec3 diffuseSampled, float specularMask) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;
    
    float dist = distance(light.position, fragPosition);
    float distSqr = dist * dist;
    vec3 direction = normalize(light.position - fragPosition);
    float diffuseAmount = max(dot(norm, direction), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse.rgb;
    
    vec3 r = reflect(-direction, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = specularMask * shineFactor * light.specular.rgb;

    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * distSqr);
    
    AMBIENT *= attenuation;
    DIFFUSE *= attenuation;
    SPECULAR *= attenuation;

    return (
        AMBIENT * diffuseSampled +
        DIFFUSE * diffuseSampled * light.diffuse.a +
        SPECULAR * material.specular.rgb * material.specular.a * light.specular.a
    );
}

vec4 triplanar(sampler2D tex, vec3 blending, vec3 coords, float worldScale) {
    vec4 xAxis = texture2D(tex, coords.yz * worldScale);
    vec4 yAxis = texture2D(tex, coords.xz * worldScale);
    vec4 zAxis = texture2D(tex, coords.xy * worldScale);
    return blending.x * xAxis + blending.y * yAxis + blending.z * zAxis;
    return vec4(0);
}

float heightRaw(vec2 position) {
	return texture(terrainFrag.heightmap, position).r;
}

float height(vec2 position) {
	float baseHeight = heightRaw(position);
	vec4 weights = splatWeight(texture(terrainFrag.splatmap, position));

	/// WAVE VARIANT 1
	//float waveHeight = 0.15;
	//float s = (sin(time + i_vertex.x*10 + i_vertex.x * time + i_vertex.z*0) + 1.0) / 2.0;
	//float s2 = (sin(time*4 + i_vertex.x*5 + i_vertex.z*20) + 1.0) / 2.0;
	//float s3 = (sin(time*8 + i_vertex.x*5 + i_vertex.z*1) + 1.0) / 2.0;
	//float s4 = (cos(time*16 + i_vertex.x*50) + 1.0) / 2.0;
	//float s5 = (sin(time + 3.141592) + 1.0) / 2.0;
	//return baseHeight + (waveHeight * 0.1 * s5 + waveHeight * 0.6 * s + waveHeight * 0.2 * s2 + waveHeight * 0.07 * s3 + waveHeight * 0.03 * s4) * weights.g;
	
	/// WAVE VARIANT 2
	float waveHeight = 0.1;
	float noise1 = 0.6 * (1.0 + simplex(vec3(4 * position, time * 0.3))) / 2.0;
	float noise2 = 0.25 * (1.0 + simplex(vec3(8 * position, time * 0.5))) / 2.0;
	float noise3 = 0.15 * (1.0 + simplex(vec3(16 * position, time * 0.47))) / 2.0;
	return baseHeight + (noise1 + noise2 + noise3) * waveHeight * weights.g * weights.g;

	/// WAVE VARIANT 3
	//float waveHeight = 0.4;
	//float wave = (sin(time*2 + position.x*20 + position.y*20) + 1.0) / 2.0;
	//return baseHeight + waveHeight * wave * weights.g;
}

vec3 calculateNormalB() {
	float hLeft    = height(uv + vec2(-0.01, 0));
	float hRight   = height(uv + vec2(0.01, 0));
	float hUp      = height(uv + vec2(0, -0.01));
	float hDown    = height(uv + vec2(0, 0.01));
	float diffA = hLeft - hRight;
	float diffB = hUp - hDown;
	return normalize(vec3(diffA, terrainFrag.uvS, -diffB));
}

void main() {
    // Precompute common values
    vec3 norm;
    if(shouldTriplanar)
        norm = normalize(normal);
    else
        norm = calculateNormalB();
    vec3 view = normalize(eye - fragPosition);

    vec3 blending = normalize(max(abs(normal), 0.000001));
    float sum = blending.x + blending.y + blending.z;
    blending /= sum;
    
    vec4 splatmap = texture2D(terrainFrag.splatmap, uv);
    vec3 weights = splatmap.rgb;
    float baseWeight = clamp(1.0 - (weights.r + weights.g + weights.b), 0, 1);
    vec4 baseTexture;
    vec4 textureR;
    vec4 textureB;
    vec4 textureWaterA;
    vec4 textureWaterB;
    vec4 textureWaterC;
    vec4 foamTex;

    baseTexture = triplanar(terrainFrag.baseTexture, blending, fragPosition, 0.7/terrainFrag.baseSize);
    textureR = triplanar(terrainFrag.textureR, blending, fragPosition, 0.7/terrainFrag.sizeR);
    textureB = triplanar(terrainFrag.textureB, blending, fragPosition, 0.7/terrainFrag.sizeB);
    textureWaterA = triplanar(terrainFrag.waterA,blending, (fragPosition + time*vec3(4,2, 0/*direction*/)), 0.7/terrainFrag.sizeWaterA);
    textureWaterB = triplanar(terrainFrag.waterA,blending, (fragPosition + time*vec3(-6,1, 0/*direction*/)), 0.7/terrainFrag.sizeWaterA * 0.7);
    textureWaterC = triplanar(terrainFrag.waterA,blending, (fragPosition + time*vec3(-1,-7, 0/*direction*/)), 0.7/terrainFrag.sizeWaterA * 4);
    foamTex = triplanar(terrainFrag.waterB, blending, (fragPosition + 0.15 * vec3(sin(time * 0.45) * 0.05, cos(time * 0.75 + 2.6) * 0.05, 0)), 0.7 / terrainFrag.sizeWaterB);


    // blend water
    float foamWeight = 0.0 + 0.0 * (sin(time*4) + 1.0) / 2;
    float texAWeight = 1 - foamWeight;
    float texAAW = 0.6;
    float texABW = 0.25;
    float texACW = 0.15;
    vec4 textureWater = texAWeight * (texAAW * textureWaterA + texABW * textureWaterB + texACW * textureWaterC) + foamWeight * foamTex;

    vec3 diffuseTexture = vec3(baseWeight * baseTexture + weights.r * textureR + weights.g * textureWater + weights.b * textureB);
    float specularMask = weights.g + weights.b;//1.0; 

    vec3 light = vec3(0);

    for(int i = 0; i < lightCount; i++) {
        if(lights[i].type == DIRECTIONAL) {
            light += DirectionalLight(lights[i], norm, view, diffuseTexture, specularMask);
        } else if(lights[i].type == POINT) {
            light += PointLight(lights[i], norm, view, diffuseTexture, specularMask);
        } else if(lights[i].type == SPOTLIGHT) {
            light += Spotlight(lights[i], norm, view, diffuseTexture, specularMask);
        }
    }

    FragColor = vec4(light, 1);
}