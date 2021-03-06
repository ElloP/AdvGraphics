#version 420 compatibility
in float life;

uniform float time;

layout(binding = 0) uniform sampler2D colortexture;
layout(binding = 1) uniform sampler2D depthbuffer;

vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	return texture(tex, rectangleCoord / textureSize(tex, 0));
}

void main()
{   
	float particleDepth = gl_FragCoord.z;
	float sceneDepth = textureRect(depthbuffer, gl_FragCoord.xy).x;

    if(sceneDepth < particleDepth){
        return;
    }

    // Base color.
    vec4 tex = texture(colortexture, gl_PointCoord);
    gl_FragColor = vec4(tex.xyz, tex.w);
    // Make it darker the older it is.
    gl_FragColor.xyz *= (min(1.0-life/3, 0.1f)) * 10;
    // Make it fade out the older it is, also all particles have a 
    // very low base alpha so they blend together.
    gl_FragColor.w = mix(tex.w, 0.0f,1-life/3);
    gl_FragColor.w * (1.0-pow(life, 4.0))*0.05;


	gl_FragColor.w *= clamp((sceneDepth - particleDepth) * 5, 0, 1);
}