#version 420

// required by GLSL spec Sect 4.5.3 (though nvidia does not, amd does)
precision highp float;

#define M_PI 3.1415926535897932384626433832795

///////////////////////////////////////////////////////////////////////////////
// Material
///////////////////////////////////////////////////////////////////////////////
vec3 material_color;
uniform float material_reflectivity;
uniform float material_metalness;
uniform float material_fresnel;
uniform float material_shininess;
uniform float material_emission;

uniform int has_emission_texture;
uniform int has_color_texture;
layout(binding = 0) uniform sampler2D colorMap;
layout(binding = 5) uniform sampler2D emissiveMap;
layout(binding = 11) uniform sampler2D diffuseMap;
///////////////////////////////////////////////////////////////////////////////
// Environment
///////////////////////////////////////////////////////////////////////////////
layout(binding = 6) uniform sampler2D environmentMap;
layout(binding = 7) uniform sampler2D irradianceMap;
layout(binding = 8) uniform sampler2D reflectionMap;
uniform float environment_multiplier;

///////////////////////////////////////////////////////////////////////////////
// Light source
///////////////////////////////////////////////////////////////////////////////
uniform vec3 point_light_color = vec3(1.0, 1.0, 1.0);
uniform float point_light_intensity_multiplier = 50.0;

///////////////////////////////////////////////////////////////////////////////
// Constants
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265359

///////////////////////////////////////////////////////////////////////////////
// Input varyings from vertex shader
///////////////////////////////////////////////////////////////////////////////
in vec2 texCoord;
in vec3 viewSpaceNormal;
in vec3 viewSpacePosition;

///////////////////////////////////////////////////////////////////////////////
// Input uniform variables
///////////////////////////////////////////////////////////////////////////////
uniform mat4 viewInverse;
uniform vec3 viewSpaceLightPosition;

///////////////////////////////////////////////////////////////////////////////
// Output color
///////////////////////////////////////////////////////////////////////////////
layout(location = 0) out vec4 fragmentColor;

vec3 calculateDirectIllumiunation(vec3 wo, vec3 n)
{
	///////////////////////////////////////////////////////////////////////////
	// Task 1.2 - Calculate the radiance Li from the light, and the direction
	//            to the light. If the light is backfacing the triangle, 
	//            return vec3(0); 
	///////////////////////////////////////////////////////////////////////////
	float d = distance(viewSpacePosition, viewSpaceLightPosition);
	vec3 Li = point_light_intensity_multiplier * point_light_color * 1 / (d*d);
	vec3 wi = normalize(viewSpaceLightPosition - viewSpacePosition);
	float ndotwi = dot(n, wi);

	if(ndotwi <= 0) {
		return vec3(0.0f);
	}

	///////////////////////////////////////////////////////////////////////////
	// Task 1.3 - Calculate the diffuse term and return that as the result
	///////////////////////////////////////////////////////////////////////////
	// vec3 diffuse_term = ...
	
	vec3 diffuse_term = material_color * (1/M_PI) * abs(ndotwi) * Li;

	///////////////////////////////////////////////////////////////////////////
	// Task 2 - Calculate the Torrance Sparrow BRDF and return the light 
	//          reflected from that instead
	///////////////////////////////////////////////////////////////////////////

	
	vec3 wh = normalize(wi + wo);
	float s = material_shininess;
	float R0 = material_fresnel;
	float ndotwo = dot(n, wo);
	float ndotwh = dot(n, wh);
	float wodotwh = dot(wo,wh);
	float whdotwi = dot(wh,wi);

	float F = R0 + (1 - R0) * pow(1 - whdotwi, 5);
	float D = pow(ndotwh, s) * (s + 2) / (2 * M_PI);
	float G = min(1, min(2 * ((ndotwh * ndotwo) / wodotwh), 2 * ((ndotwh * ndotwi) / wodotwh)));
	float brdf = F * D * G / (4 * ndotwo * ndotwi);


	///////////////////////////////////////////////////////////////////////////
	// Task 3 - Make your shader respect the parameters of our material model.
	///////////////////////////////////////////////////////////////////////////
	float m = material_metalness;
	float r = material_reflectivity;

	vec3 dielectric_term = brdf * ndotwi * Li + (1-F) * diffuse_term;
	vec3 metal_term = brdf * material_color * ndotwi * Li;
	vec3 microfacet_term = m * metal_term + (1 - m) * dielectric_term;
	return r * microfacet_term + (1 - r) * diffuse_term;
}

vec3 calculateIndirectIllumination(vec3 wo, vec3 n)
{
	///////////////////////////////////////////////////////////////////////////
	// Task 5 - Lookup the irradiance from the irradiance map and calculate
	//          the diffuse reflection
	///////////////////////////////////////////////////////////////////////////

	vec3 nws = normalize(viewInverse * vec4(n,0)).xyz;

	// Calculate the spherical coordinates of the direction
	float theta = acos(max(-1.0f, min(1.0f, nws.y)));
	float phi = atan(nws.z, nws.x);
	if (phi < 0.0f) phi = phi + 2.0f * M_PI;

	// Use these to lookup the color in the environment map
	vec2 lookup = vec2(phi / (2.0 * M_PI), theta / M_PI);



	vec3 irradiance = vec3(environment_multiplier * texture(irradianceMap, lookup));
	vec3 diffuse_term = material_color * (1.0f / M_PI) * irradiance;

	///////////////////////////////////////////////////////////////////////////
	// Task 6 - Look up in the reflection map from the perfect specular 
	//          direction and calculate the dielectric and metal terms. 
	///////////////////////////////////////////////////////////////////////////
	vec3 wi = normalize(viewInverse * vec4(reflect(-wo, n),0)).xyz;

	// Calculate the spherical coordinates of the direction
	theta = acos(max(-1.0f, min(1.0f, wi.y)));
	phi = atan(wi.z, wi.x);
	if (phi < 0.0f) phi = phi + 2.0f * M_PI;

	// Use these to lookup the color in the environment map
	lookup = vec2(phi / (2.0 * M_PI), theta / M_PI);

	vec3 wh = normalize(wi + wo);
	float s = material_shininess;
	float roughness = sqrt(sqrt(2/(s + 2)));
	float R0 = material_fresnel;
	float m = material_metalness;
	float r = material_reflectivity;
	float F = R0 + (1 - R0) * pow(1 - dot(wh, wi), 5);

	vec3 Li = environment_multiplier * textureLod(reflectionMap, lookup, roughness * 7.0).xyz;
	//vec3 diffuse_term = material_color * (1/M_PI) * abs(dot(n, wi)) * Li;
	vec3 dielectric_term = F * Li + (1 - F) * diffuse_term;
	vec3 metal_term = F * material_color * Li;
	vec3 microfacet_term = m * metal_term + (1 - m) * dielectric_term;
	return r * microfacet_term + (1 - r) * diffuse_term;
}

void main() 
{
	material_color = texture(diffuseMap, texCoord).xyz;
	///////////////////////////////////////////////////////////////////////////
	// Task 1.1 - Fill in the outgoing direction, wo, and the normal, n. Both
	//            shall be normalized vectors in view-space. 
	///////////////////////////////////////////////////////////////////////////
	vec3 wo = normalize(-viewSpacePosition);
	vec3 n = normalize(viewSpaceNormal);

	vec3 direct_illumination_term = vec3(0.0);
	{ // Direct illumination
		direct_illumination_term = calculateDirectIllumiunation(wo, n);
	}

	vec3 indirect_illumination_term = vec3(0.0);
	{ // Indirect illumination
		indirect_illumination_term = calculateIndirectIllumination(wo, n);
	}

	///////////////////////////////////////////////////////////////////////////
	// Task 7 - Make glowy things glow!
	///////////////////////////////////////////////////////////////////////////
	vec3 emission_term = material_emission * material_color;

	fragmentColor.xyz =
		direct_illumination_term +
		indirect_illumination_term +
		emission_term;

}
