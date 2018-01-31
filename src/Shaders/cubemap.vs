#version 420 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main()
{
	gl_Position = (projection * view  * vec4(position.x, -position.y, position.z, 1.0f)).xyww;
	texCoords = position;
}