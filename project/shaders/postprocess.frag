#version 420 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

uniform float time;

layout(binding = 0) uniform sampler2D frameBufferTexture;
layout(binding = 1) uniform sampler2D hazeBufferTexture;
out vec4 color;


vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}

void main()
{
	vec2 texSize = textureSize(frameBufferTexture, 0);
    vec4 frame = textureRect(frameBufferTexture, gl_FragCoord.xy);
    vec4 haze = textureRect(hazeBufferTexture, gl_FragCoord.xy);

	vec2 distortion = sin(time * haze.xy);
	vec2 distorted =  gl_FragCoord.xy + distortion;
    
    color = haze.x == 0 ? frame : textureRect(frameBufferTexture, distorted) + haze/4;
}