#version 420 core

precision highp float;

layout (binding = 0) uniform samplerCube mapTexture;

out vec4 FragColor;

in vec3 texCoords;

#define PI 3.14159265359
void main()
{
	FragColor = texture(mapTexture, texCoords);
}