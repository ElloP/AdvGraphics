#version 420
///////////////////////////////////////////////////////////////////////////////
// Input vertex attributes
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoordIn;


///////////////////////////////////////////////////////////////////////////////
// Heightmap
///////////////////////////////////////////////////////////////////////////////
layout(binding = 10) uniform sampler2D heightMapTex;

uniform vec3 viewSpaceLightPosition;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;

///////////////////////////////////////////////////////////////////////////////
// Output to fragment shader
///////////////////////////////////////////////////////////////////////////////
out vec2 texCoord;
out vec3 viewSpacePosition;
out vec3 viewSpaceNormal;

float getHeightOffset(vec2 heightMapCoord) {
	return texture(heightMapTex, heightMapCoord).x;
}

vec2 uvCoords(vec2 xzCoords){
	return (1.0 + xzCoords) / 2.0;
}

void main() 
{
	vec2 textureSize2d = textureSize(heightMapTex, 0);

	float heightOffset = getHeightOffset(texCoordIn);
	vec3 actualPosition = vec3(position.x, heightOffset, position.z);

	vec2 pixelX = vec2(1.0f / textureSize2d.x, 0);
	vec2 pixelY = vec2(0, 1.0f / textureSize2d.y);

	float offset = 0.01;
	vec2 offsets = uvCoords(vec2(position.x + offset, position.z + offset));

	vec3 first = vec3(position.x + offset,getHeightOffset(vec2(offsets.x,texCoordIn.y)), position.z);
	vec3 second = vec3(position.x,getHeightOffset(vec2(texCoordIn.x,offsets.y)), position.z + offset);

	vec4 normalIn = vec4(cross(normalize(first - actualPosition),normalize(second - actualPosition)),1.0);

	gl_Position = (modelViewProjectionMatrix * vec4(actualPosition, 1.0f));// + vec4(normalize((normalMatrix * normalIn).xyz),1.0);
	
	texCoord = texCoordIn;
		
	viewSpaceNormal = -normalize((normalMatrix * normalIn).xyz);
	viewSpacePosition = (modelViewMatrix * gl_Position).xyz;
}
