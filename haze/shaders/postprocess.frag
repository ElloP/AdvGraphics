#version 420 core

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

uniform float time;
uniform float magnitude;
uniform bool particles;

layout(binding = 0) uniform sampler2D frameBufferTexture;
layout(binding = 1) uniform sampler2D hazeBufferTexture;

out vec4 color;


vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	vec2 texCoord = rectangleCoord / textureSize(tex, 0);

	return texture(tex, texCoord);
}

vec4 particleRect(in sampler2D tex, vec2 rectangleCoord)
{
	vec2 texCoord = rectangleCoord / textureSize(tex, 0);
	texCoord.x = clamp(texCoord.x, 0 , 1);
	texCoord.y = clamp(texCoord.y, 0 , 1);

	return texture(tex, texCoord);
}

void main()
{
    vec4 frame = textureRect(frameBufferTexture, gl_FragCoord.xy);
    vec4 haze = textureRect(hazeBufferTexture, gl_FragCoord.xy);

	vec2 distortion = sin(time * haze.xy) * magnitude;
	vec2 distorted =  gl_FragCoord.xy + distortion;
    
	color = frame;
	if(haze.x != 0){
		color = (frame + particleRect(frameBufferTexture, distorted)) / 2;
		if(particles) {
			color *= haze.w;
			color += haze;
		}
	}
}