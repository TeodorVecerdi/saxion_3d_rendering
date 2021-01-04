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

vec3 ambient() {
    return ambientColor.a * ambientColor.rgb;
}

vec3 diffuse(in vec3 norm, out vec3 lightDir) {
    float lightDistance = distance(lightPosition, fragPosition);
    float attn; 
    if(lightType == 0) { // directional
        lightDir = normalize(-lightDirection);
        attn = 1;
    } else if (lightType == 1) {// point
        attn = attenuation.x + attenuation.y * lightDistance + attenuation.z * lightDistance * lightDistance;
        lightDir = normalize(lightPosition - fragPosition);
    } else if (lightType == 2) {// spot, same as directional for now
        lightDir = normalize(-lightDirection);
        attn = 1;
    }
    
    float diffuseAmount = max(dot(norm, lightDir), 0.0);
    // apply attenuation
    diffuseAmount /= attn;
    
    return diffuseAmount * lightColor;
}

vec3 specular(in vec3 norm, in vec3 lightDir) {
    vec3 R = reflect(-lightDir, norm);
    vec3 V = normalize(eye - fragPosition);
    return specularColor.a * pow(max(dot(R, V), 0), shininess) * lightColor * specularColor.rgb;
}


void main() {
    vec3 norm = normalize(normal);
    vec3 lightDir;

    /// ambient
    vec3 ambientFactor = ambient();
    
    /// diffuse
    vec3 diffuseFactor = diffuse(norm, lightDir);

    /// specular
    vec3 specularFactor = specular(norm, lightDir);
    vec3 tex = vec3(texture(diffuseTexture, uv));
    vec3 result = (ambientFactor + diffuseFactor + specularFactor) * tex;

    gl_FragColor = vec4(result, 1.0);
}