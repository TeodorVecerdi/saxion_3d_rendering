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
    sampler2D textureG;
    float sizeG;
    sampler2D textureB;
    float sizeB;
    sampler2D textureA;
    float sizeA;
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
    vec4 baseTexture = texture2D(terrainFrag.baseTexture, uv * terrainFrag.baseSize);
    vec4 textureR = texture2D(terrainFrag.textureR, uv * terrainFrag.sizeR);
    vec4 textureG = texture2D(terrainFrag.textureG, uv * terrainFrag.sizeG);
    vec4 textureB = texture2D(terrainFrag.textureB, uv * terrainFrag.sizeB);
    vec4 textureA = texture2D(terrainFrag.textureA, uv * terrainFrag.sizeA);

    vec4 weights = splatmap;
    float sum = weights.r + weights.g + weights.b + weights.a;
    weights /= sum;
    float baseWeight = 1 - (weights.r + weights.g + weights.b + weights.a);

    vec3 diffuseTexture = vec3(baseWeight * baseTexture + weights.r * textureR + weights.g * textureG + weights.b * textureB + weights.a * textureA);
    float specularMask = weights.g;

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

    //light = normalize(light);

    FragColor = vec4(light, 1);
}