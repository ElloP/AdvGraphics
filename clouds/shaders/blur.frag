#version 420

out vec4 fcolor;

layout (binding = 0) uniform sampler2D image;


uniform bool horizontal;

vec4 textureRect(in sampler2D tex, vec2 rectangleCoord)
{
    vec2 texCoord = rectangleCoord / textureSize(tex, 0);

    return texture(tex, texCoord);
}

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
	
    vec2 tex_offset = vec2(1.0);
    vec4 result = textureRect(image, gl_FragCoord.xy).rgba * weight[0];

    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += textureRect(image, gl_FragCoord.xy + vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
            result += textureRect(image, gl_FragCoord.xy - vec2(tex_offset.x * i, 0.0)).rgba * weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += textureRect(image, gl_FragCoord.xy + vec2(0.0, tex_offset.y * i)).rgba * weight[i];
            result += textureRect(image, gl_FragCoord.xy - vec2(0.0, tex_offset.y * i)).rgba * weight[i];
        }
    }

    fcolor = result;
}