#version 450

#define MAX_LIGHTS 128
#define DIRECTIONAL 1
#define POINT 2
#define SPOTLIGHT 4

struct MaterialData {
    vec3 diffuse;
    vec4 specular; // rgb=color, a=intensity
    float shininess;
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

in vec3 normal;
in vec3 fragPosition;

layout (location = 4)  uniform vec3 eye;
uniform MaterialData material;

uniform uint lightCount;
uniform LightData lights[MAX_LIGHTS];

vec3 Spotlight(LightData light, vec3 norm, vec3 view) {
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
    vec3 SPECULAR = shineFactor * light.specular;
    
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * distSqr);
    AMBIENT *= attenuation;
    DIFFUSE *= attenuation;
    SPECULAR *= attenuation;

    DIFFUSE *= intensity;
    SPECULAR *= intensity;

    return (
        AMBIENT * material.diffuse +
        DIFFUSE * material.diffuse +
        SPECULAR * material.specular.rgb * material.specular.a
    );
}

vec3 DirectionalLight(LightData light, vec3 norm, vec3 view) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;
    
    vec3 direction = normalize(-light.direction);
    float diffuseAmount = max(dot(norm, direction), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse;
    
    vec3 r = reflect(-direction, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = shineFactor * light.specular;
    
    return (
        AMBIENT * material.diffuse +
        DIFFUSE * material.diffuse +
        SPECULAR * material.specular.rgb * material.specular.a
     );
}

vec3 PointLight(LightData light, vec3 norm, vec3 view) {
    vec3 AMBIENT = light.ambient.a * light.ambient.rgb;
    
    float dist = distance(light.position, fragPosition);
    float distSqr = dist * dist;
    vec3 direction = normalize(light.position - fragPosition);
    float diffuseAmount = max(dot(norm, direction), 0.0);
    vec3 DIFFUSE = diffuseAmount * light.diffuse;
    
    vec3 r = reflect(-direction, norm);
    float shineFactor = pow(max(dot(r, view), 0), material.shininess);
    vec3 SPECULAR = shineFactor * light.specular;

    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * dist + light.attenuation.z * distSqr);
    
    AMBIENT *= attenuation;
    DIFFUSE *= attenuation;
    SPECULAR *= attenuation;

    return (
        AMBIENT * material.diffuse +
        DIFFUSE * material.diffuse +
        SPECULAR * material.specular.rgb * material.specular.a
    );
}


void main() {
    // Precompute common values
    vec3 norm = normalize(normal);
    vec3 view = normalize(eye - fragPosition);
    // For texture lit: sample textures here

    vec3 light = vec3(0);
    for(int i = 0; i < lightCount; i++) {
        if(lights[i].type == DIRECTIONAL) {
            light += DirectionalLight(lights[i], norm, view);
        } else if(lights[i].type == POINT) {
            light += PointLight(lights[i], norm, view);
        } else if(lights[i].type == SPOTLIGHT) {
            light += Spotlight(lights[i], norm, view);
        }
    }

    gl_FragColor = vec4(light, 1);
}