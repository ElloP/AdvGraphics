#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;

void main()
{
	texCoords = 0.5 * (position + 1);
	gl_Position = vec4(position,1.0,1.0);
}