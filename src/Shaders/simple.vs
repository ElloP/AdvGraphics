#version 420 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 viewSpaceNormal;
out vec3 viewSpacePosition;
out vec2 outTexCoords;

void main()
{
    outTexCoords = texCoords;
    mat4 modelView = view * model;
    mat4 normalMatrix = transpose(inverse(modelView));
    viewSpaceNormal = (normalMatrix * vec4(normal, 0.0f)).xyz;
    viewSpacePosition = (modelView * vec4(pos, 0.0f)).xyz;
    gl_Position = projection * view * model * vec4(pos, 1.0f);
}