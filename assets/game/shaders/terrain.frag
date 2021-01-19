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
    float uvS;
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

vec4 splatWeight(vec4 splatTex) {
	vec4 weight = vec4(splatTex.xyz, 0);
	weight.w = clamp(1.0 - (weight.x + weight.y + weight.z), 0.0, 1.0);
	return weight;
}

float heightRaw(vec2 position) {
	return texture(terrainFrag.heightmap, position).r;
}

float height(vec2 position) {
	float h = heightRaw(position);
	vec4 weights = splatWeight(texture(terrainFrag.splatmap, position));
	
	float waveHeight = 0.15;
	float s = (sin(time + fragPosition.x*10 + fragPosition.x * time + fragPosition.z*0) + 1.0) / 2.0;
	float s2 = (sin(time*4 + fragPosition.x*5 + fragPosition.z*20) + 1.0) / 2.0;
	float s3 = (sin(time*8 + fragPosition.x*5 + fragPosition.z*1) + 1.0) / 2.0;
	float s4 = (cos(time*16 + fragPosition.x*50) + 1.0) / 2.0;
	float s5 = (sin(time + 3.141592) + 1.0) / 2.0;
	return h + (waveHeight * 0.1 * s5 + waveHeight * 0.6 * s + waveHeight * 0.2 * s2 + waveHeight * 0.07 * s3 + waveHeight * 0.03 * s4) * weights.g;
	
	//float waveHeight = 0.09;
	//float noise1 = 0.6 * (1.0 + simplex(vec3(2 * fragPosition.xz, time * 0.3))) / 2.0;
	//float noise2 = 0.25 * (1.0 + simplex(vec3(8 * i_vertex.xz, time * 0.5))) / 2.0;
	//float noise3 = 0.15 * (1.0 + simplex(vec3(16 * i_vertex.xz, time * 0.47))) / 2.0;
	//return h + (noise1 + noise2 + noise3) * waveHeight * weights.g * weights.g;
}

vec3 calcNormal() {
    float hD = height(uv + 0.01 * vec2(0,-1));
	float hL = height(uv + 0.01 * vec2(-1,0));
	float hR = height(uv + 0.01 * vec2(1,0));
	float hU = height(uv + 0.01 * vec2(0,1));
	return normalize(vec3(hL-hR, terrainFrag.uvS, hD - hU));
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

void main() {
    // Precompute common values
    vec3 norm = normalize(normal);
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

    if(shouldTriplanar) {
        baseTexture = triplanar(terrainFrag.baseTexture, blending, fragPosition, 0.7/terrainFrag.baseSize);
        textureR = triplanar(terrainFrag.textureR, blending, fragPosition, 0.7/terrainFrag.sizeR);
        textureB = triplanar(terrainFrag.textureB, blending, fragPosition, 0.7/terrainFrag.sizeB);
        textureWaterA = triplanar(terrainFrag.waterA,blending, (fragPosition + time*vec3(4,2, 0/*direction*/)), 0.7/terrainFrag.sizeWaterA);
        textureWaterB = triplanar(terrainFrag.waterA,blending, (fragPosition + time*vec3(-6,1, 0/*direction*/)), 0.7/terrainFrag.sizeWaterA * 0.7);
        textureWaterC = triplanar(terrainFrag.waterA,blending, (fragPosition + time*vec3(-1,-7, 0/*direction*/)), 0.7/terrainFrag.sizeWaterA * 4);
        foamTex = triplanar(terrainFrag.waterB, blending, (fragPosition + 0.15 * vec3(sin(time * 0.45) * 0.05, cos(time * 0.75 + 2.6) * 0.05, 0)), 0.7 / terrainFrag.sizeWaterB);
    } else {
        baseTexture = texture2D(terrainFrag.baseTexture, uv * terrainFrag.baseSize);
        textureR = texture2D(terrainFrag.textureR, uv * terrainFrag.sizeR);
        textureB = texture2D(terrainFrag.textureB, uv * terrainFrag.sizeB);
        textureWaterA = texture2D(terrainFrag.waterA, (uv + time*vec2(0.02,0.01/*direction*/)) * terrainFrag.sizeWaterA);
        textureWaterB = texture2D(terrainFrag.waterA, (uv + 0.7*time*vec2(-0.02,0.01/*direction*/)) * terrainFrag.sizeWaterA * 0.7);
        textureWaterC = texture2D(terrainFrag.waterA, (uv + 0.4*time*vec2(-0.02,-0.01/*direction*/)) * terrainFrag.sizeWaterA * 4);
        foamTex = texture2D(terrainFrag.waterB, (uv + 0.15 * vec2(sin(time * 0.45) * 0.05, cos(time * 0.75 + 2.6) * 0.05)) * terrainFrag.sizeWaterB);
    }

    // blend water
    float foamWeight = 0.0 + 0.0 * (sin(time*4) + 1.0) / 2;
    float texAWeight = 1 - foamWeight;
    float texAAW = 0.6;
    float texABW = 0.25;
    float texACW = 0.15;
    vec4 textureWater = texAWeight * (texAAW * textureWaterA + texABW * textureWaterB + texACW * textureWaterC) + foamWeight * foamTex;

    vec3 diffuseTexture = vec3(baseWeight * baseTexture + weights.r * textureR + weights.g * textureWater + weights.b * textureB);

    float specularMask = weights.g + weights.b;

    diffuseTexture = vec3(0.8);

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

    FragColor = vec4(abs(norm), 1);
}