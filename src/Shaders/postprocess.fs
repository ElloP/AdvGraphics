#version 420 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

uniform float time;

layout(binding = 0) uniform sampler2D frameBufferTexture;
layout(binding = 1) uniform sampler2D hazeBufferTexture;
layout(binding = 2) uniform sampler2D depthBufferTexture;
out vec4 color;


vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}

void main()
{

    vec4 frame = textureRect(frameBufferTexture, gl_FragCoord.xy);
    vec4 haze = textureRect(hazeBufferTexture, gl_FragCoord.xy);
    vec4 depth = textureRect(depthBufferTexture, gl_FragCoord.xy);
    
    color = haze.x == 0 ? frame : textureRect(frameBufferTexture, gl_FragCoord.xy + sin(time * haze.xy) * 10) + haze;
    

    //color = haze;
    //color = vec4(depth.x);
    //color = frame;
}