#version 420 core

layout(binding = 0) uniform sampler2D colortexture;
layout(binding = 1) uniform sampler2D depthbuffer;

uniform vec3 view_port;

out vec4 color;

vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
	vec2 texCoord = rectangleCoord / textureSize(tex, 0);

	return texture(tex, texCoord);
}

void main(){
	float colorDepth = gl_FragCoord.z;
	//float sceneDepth = textureRect(depthbuffer, gl_FragCoord.xy).x;

    // Base color.
    vec4 tex = textureRect(colortexture, gl_FragCoord.xy);

    if(tex.a == 0.0f){
    	discard;
    }

    vec2 textureSize2d = textureSize(colortexture, 0);

    float xoffset = 1.0f ;
    float yoffset = 1.0f ;
/*
    vec4 samp1 = textureRect(colortexture,vec2(gl_FragCoord.x + xoffset,gl_FragCoord.y + yoffset));
    vec4 samp2 = textureRect(colortexture,vec2(gl_FragCoord.x + xoffset,gl_FragCoord.y - yoffset));
    vec4 samp3 = textureRect(colortexture,vec2(gl_FragCoord.x - xoffset,gl_FragCoord.y + yoffset));
    vec4 samp4 = textureRect(colortexture,vec2(gl_FragCoord.x - xoffset,gl_FragCoord.y - yoffset));
    vec4 samp5 = textureRect(colortexture,vec2(gl_FragCoord.x,gl_FragCoord.y + yoffset));
    vec4 samp6 = textureRect(colortexture,vec2(gl_FragCoord.x + xoffset,gl_FragCoord.y));
    vec4 samp7 = textureRect(colortexture,vec2(gl_FragCoord.x - xoffset,gl_FragCoord.y));
    vec4 samp8 = textureRect(colortexture,vec2(gl_FragCoord.x,gl_FragCoord.y - yoffset));

    float surrAm = 2.0/9.0;
*/
   	color = tex; //* surrAm + surrAm * (samp1 + samp2 + samp3 + samp4 + samp5 + samp6+ samp7 + samp8) ;
}