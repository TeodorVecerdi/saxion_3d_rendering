#version 450

in vec2 uv;
in vec3 normal;
in vec3 fragPosition;

// Base object data
layout (location = 4)  uniform sampler2D diffuseTexture;
layout (location = 5)  uniform vec4 ambientColor; // rgb=color, a=intensity
// Specular data
layout (location = 6)  uniform vec4  specularColor; // rgb=color, a=intensity
layout (location = 7)  uniform float shininess;
layout (location = 8)  uniform vec3  eye;
// Light data
layout (location = 9)  uniform uint lightType;
layout (location = 10) uniform vec3 lightPosition;
layout (location = 11) uniform vec3 lightDirection;
layout (location = 12) uniform vec3 lightColor;
layout (location = 13) uniform vec3 attenuation; // x = const, y = linear, z = quadratic
layout (location = 14) uniform float outerAngle;
layout (location = 15) uniform float innerAngle;

const uint DIRECTIONAL = 1;
const uint POINT = 2;
const uint SPOTLIGHT = 4;


vec3 lightWeight() {
    return vec3((lightType & DIRECTIONAL)/DIRECTIONAL, (lightType & POINT)/POINT, (lightType & SPOTLIGHT)/SPOTLIGHT);
}

vec3 pointLight() {
    vec3 norm = normalize(normal);
    vec3 AMBIENT = ambientColor.a * ambientColor.rgb;
    float dist = distance(lightPosition, fragPosition);
    float distSqr = dist * dist;
    vec3 lightDir = normalize(lightPosition - fragPosition);
    float attn = 1.0 / (attenuation.x + attenuation.y * dist + attenuation.z * distSqr);
    float diffuseAmount = max(dot(norm, lightDir), 0.0);
    vec3 DIFFUSE = diffuseAmount * attn * lightColor;
    vec3 r = reflect(-lightDir, norm);
    vec3 v = normalize(eye - fragPosition);
    float shineFactor = pow(max(dot(r, v), 0), shininess);
    vec3 SPECULAR = specularColor.a * attn * shineFactor * lightColor * specularColor.rgb;
    return (AMBIENT + DIFFUSE + SPECULAR);
}

vec3 spotlight() {
    vec3 norm = normalize(normal);
    vec3 AMBIENT = ambientColor.a * ambientColor.rgb;
    
    vec3 DIFFUSE = vec3(0);
    vec3 SPECULAR = vec3(0);

    vec3 lightDir = normalize(lightPosition - fragPosition);
    
    float theta = dot(lightDir, normalize(-lightDirection));
    float epsilon = innerAngle - outerAngle;
    float intensity = clamp((theta-outerAngle)/epsilon,0.0,1.0);
    
    float dist = distance(lightPosition, fragPosition);
    float distSqr = dist * dist;
    float attn = 1.0 / (attenuation.x + attenuation.y * dist + attenuation.z * distSqr);
    float diffuseAmount = max(dot(norm, lightDir), 0.0);
    DIFFUSE = diffuseAmount * lightColor;
    
    vec3 r = reflect(-lightDir, norm);
    vec3 v = normalize(eye - fragPosition);
    float shineFactor = pow(max(dot(r, v), 0), shininess);
    SPECULAR = specularColor.a * shineFactor * lightColor * specularColor.rgb;

    AMBIENT *= attn;
    DIFFUSE *= attn;
    SPECULAR *= attn;

    DIFFUSE *= intensity;
    SPECULAR *= intensity;
  
    
    return (AMBIENT + DIFFUSE + SPECULAR);
}

vec3 directionalLight() {
    vec3 norm = normalize(normal);
    vec3 AMBIENT = ambientColor.a * ambientColor.rgb;
    float dist = distance(lightPosition, fragPosition);
    float distSqr = dist * dist;
    vec3 lightDir = normalize(-lightDirection);
    float diffuseAmount = max(dot(norm, lightDir), 0.0);
    vec3 DIFFUSE = diffuseAmount * lightColor;
    vec3 r = reflect(-lightDir, norm);
    vec3 v = normalize(eye - fragPosition);
    float shineFactor = pow(max(dot(r, v), 0), shininess);
    vec3 SPECULAR = specularColor.a * shineFactor * lightColor * specularColor.rgb;
    return (AMBIENT + DIFFUSE + SPECULAR);
}


void main() {
    vec3 weights = lightWeight();
    
    vec3 directionalVal = directionalLight() * weights.x;
    vec3 pointVal = pointLight() * weights.y;
    vec3 spotlightVal = spotlight() * weights.z;

    vec3 tex = vec3(texture(diffuseTexture, uv));
    vec3 result = (directionalVal + pointVal + spotlightVal) * tex;

    gl_FragColor = vec4(result, 1.0);
}