#version 420 compatibility
in float life;
in vec2 position;

uniform float time;

layout(binding = 0) uniform sampler2D colortexture;


void main()
{   
    // Base color.
    vec4 tex = texture(colortexture, gl_PointCoord);
    gl_FragColor = vec4(tex.xxx, tex.w);
    // Make it darker the older it is.
    gl_FragColor.xyz *= (1.0-life);
    // Make it fade out the older it is, also all particles have a 
    // very low base alpha so they blend together.
    gl_FragColor.w = mix(tex.w, 0.0f, 1 - life/2.0f);//gl_FragColor.w * (1.0-pow(life, 4.0))*0.05;
}