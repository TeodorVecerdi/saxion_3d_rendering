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
    vec3 diffuse;
    vec3 specular;

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

uniform uint lightCount;
uniform LightData lights[MAX_LIGHTS];


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
    vec3 DIFFUSE = diffuseAmount * light.diffuse;

    vec3 r = reflect(-lightDir, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = specularMask * shineFactor * light.specular;
    
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * distSqr);
    AMBIENT *= attenuation;
    DIFFUSE *= attenuation;
    SPECULAR *= attenuation;

    DIFFUSE *= intensity;
    SPECULAR *= intensity;

    return (
        AMBIENT * diffuseSampled +
        DIFFUSE * diffuseSampled +
        SPECULAR * material.specular.rgb * material.specular.a
    );
}

vec3 DirectionalLight(LightData light, vec3 norm, vec3 view, vec3 diffuseSampled, float specularMask) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;
    
    vec3 direction = normalize(-light.direction);
    float diffuseAmount = max(dot(norm, direction), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse;
    
    vec3 r = reflect(-direction, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = specularMask * shineFactor * light.specular;
    
    return (
        AMBIENT * diffuseSampled +
        DIFFUSE * diffuseSampled +
        SPECULAR * material.specular.rgb * material.specular.a
     );
}

vec3 PointLight(LightData light, vec3 norm, vec3 view, vec3 diffuseSampled, float specularMask) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;
    
    float dist = distance(light.position, fragPosition);
    float distSqr = dist * dist;
    vec3 direction = normalize(light.position - fragPosition);
    float diffuseAmount = max(dot(norm, direction), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse;
    
    vec3 r = reflect(-direction, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = specularMask * shineFactor * light.specular;

    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * distSqr);
    
    AMBIENT *= attenuation;
    DIFFUSE *= attenuation;
    SPECULAR *= attenuation;

    return (
        AMBIENT * diffuseSampled +
        DIFFUSE * diffuseSampled +
        SPECULAR * material.specular.rgb * material.specular.a
    );
}

void main() {
    // Precompute common values
    vec3 norm = normalize(normal);
    vec3 view = normalize(eye - fragPosition);
    
    vec4 splatmap = texture2D(terrainFrag.splatmap, uv);
    vec3 weights = splatmap.rgb;
    float baseWeight = clamp(1.0 - (weights.r + weights.g + weights.b), 0, 1);
    
    vec4 baseTexture = texture2D(terrainFrag.baseTexture, uv * terrainFrag.baseSize);
    vec4 textureR = texture2D(terrainFrag.textureR, uv * terrainFrag.sizeR);
    vec4 textureB = texture2D(terrainFrag.textureB, uv * terrainFrag.sizeB);

    // Water
    vec4 textureWaterA = texture2D(terrainFrag.waterA, (uv + time*vec2(0.02,0.01/*direction*/)) * terrainFrag.sizeWaterA);
    vec4 textureWaterB = texture2D(terrainFrag.waterA, (uv + 0.7*time*vec2(-0.02,0.01/*direction*/)) * terrainFrag.sizeWaterA * 0.7);
    vec4 textureWaterC = texture2D(terrainFrag.waterA, (uv + 0.4*time*vec2(-0.02,-0.01/*direction*/)) * terrainFrag.sizeWaterA * 4);
    vec4 foamTex = texture2D(terrainFrag.waterB, (uv + 0.15 * vec2(sin(time * 0.45) * 0.05, cos(time * 0.75 + 2.6) * 0.05)) * terrainFrag.sizeWaterB);
   
    // blend water
    float foamWeight = 0.0 + 0.0 * (sin(time*4) + 1.0) / 2;
    float texAWeight = 1 - foamWeight;
    float texAAW = 0.6;
    float texABW = 0.25;
    float texACW = 0.15;
    vec4 textureWater = texAWeight * (texAAW * textureWaterA + texABW * textureWaterB + texACW * textureWaterC) + foamWeight * foamTex;

    vec3 diffuseTexture = vec3(baseWeight * baseTexture + weights.r * textureR + weights.g * textureWater + weights.b * textureB);

    float specularMask = weights.g + weights.b;

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