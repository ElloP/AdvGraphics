#version 420 core

uniform vec4 inColor;
layout (binding = 0) uniform sampler2D ourTexture;
layout (binding = 1) uniform sampler2D ourTexture2;

in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;
in vec2 outTexCoords;

out vec4 color;

vec3 lightPos = vec3(1.2,0.0,2.0);
vec3 lightColor = vec3(1.0,1.0,1.0);

uniform float blend;

vec3 ambient()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * inColor.xyz; 
    return result;
}

vec3 diffuse(vec3 _viewSpaceNormal)
{
    float diff = max(dot(_viewSpaceNormal, normalize(lightPos - viewSpacePosition)), 0);
    vec3 diffuse = diff * lightColor;
    return diffuse;
}

vec3 specular(vec3 _viewSpaceNormal)
{
    float specularStrength = 0.5f;
    vec3 viewDir = -viewSpacePosition;
    vec3 reflectDir = reflect(normalize(lightPos - viewSpacePosition), _viewSpaceNormal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    return specular;
}

void main()
{
    vec4 texColor = mix(texture(ourTexture, outTexCoords), texture(ourTexture2,outTexCoords),blend);
    vec3 normviewSpaceNormal = normalize(viewSpaceNormal);

    vec3 objectColor = diffuse(normviewSpaceNormal) + ambient() + specular(normviewSpaceNormal);
    //vec3 result = objectColor * texColor.xyz;
    color = texColor; // + vec4(objectColor,1);//vec4(result, 1);
}