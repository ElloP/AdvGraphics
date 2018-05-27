#version 420 core

layout(location = 0) in vec2 position;

out vec3 Dposition;

void main() 
{
	gl_Position = vec4(position, 0.0, 1.0);
	Dposition = gl_Position.xyz;
}