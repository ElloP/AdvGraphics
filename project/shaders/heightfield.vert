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

void main() 
{
	vec2 textureSize2d = textureSize(heightMapTex, 0);

	float heightOffset = getHeightOffset(texCoordIn);

	vec2 pixelX = vec2(1.0f / textureSize2d.x, 0);
	vec2 pixelY = vec2(0, 1.0f / textureSize2d.y);

	float scaleY = modelViewProjectionMatrix[1][1];

	vec3 va = vec3(0, 1, (getHeightOffset(texCoordIn + pixelY) - heightOffset) * scaleY);
	vec3 vb = vec3(1, 0, (getHeightOffset(texCoordIn + pixelX) - heightOffset) * scaleY);
	vec3 vc = vec3(0, -1, (getHeightOffset(texCoordIn - pixelY) - heightOffset) * scaleY);
	vec3 vd = vec3(-1, 0, (getHeightOffset(texCoordIn - pixelX) - heightOffset) * scaleY);

	vec4 normalIn = vec4(normalize((cross(va, vb) + cross(vb,vc) + cross(vc,vd) + cross(vd, va)) / -4), 0.0f);

	gl_Position = (modelViewProjectionMatrix * vec4(vec3(position.x, heightOffset, position.z), 1.0f));
	
	texCoord = texCoordIn;
		
	viewSpaceNormal = (normalMatrix * normalIn).xyz;
	viewSpacePosition = (modelViewMatrix * gl_Position).xyz;
}
