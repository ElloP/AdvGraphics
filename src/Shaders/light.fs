#version 420 core

layout (binding = 0) uniform sampler2D diffuseMap;
layout (binding = 1) uniform sampler2D specularMap;
layout (binding = 2) uniform sampler2D normalMap;

out vec4 FragColor;

struct Material
{
	vec3 ambient; // not using
	vec3 diffuse; // not using
	vec3 specular; // not using
	float shininess;
};
struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct PointLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};
struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(lightDir,normal),0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

	vec3 ambient = light.ambient * vec3(texture(diffuseMap,TexCoords));
	vec3 diffuse = diff * light.diffuse * vec3(texture(diffuseMap,  TexCoords));
	vec3 specular = spec * light.specular * vec3(texture(specularMap, TexCoords)); 
	return (ambient + diffuse + specular);

}
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(lightDir,normal),0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(specularMap, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intesity = clamp((theta - light.outerCutoff) / epsilon,0.0,1.0);
	vec3 ambient, diffuse, specular;
	if(theta > light.outerCutoff)
	{
		float diff = max(dot(lightDir,normal),0.0);
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		ambient  = light.ambient  * vec3(texture(diffuseMap, TexCoords));
   	 	diffuse  = light.diffuse  * diff * vec3(texture(diffuseMap, TexCoords));
    	specular = light.specular * spec * vec3(texture(specularMap, TexCoords));
    	ambient *= intesity;
    	diffuse *= intesity;
    	specular *= intesity;

	}
	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	vec3 norm = normalize(Normal);
	//vec3 norm = normalize(vec3(texture(normalMap,TexCoords).rgb * 2 - 1));
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = calcDirLight(dirLight,norm,viewDir);
	result += calcPointLight(pointLight,norm,FragPos,viewDir);
	result += calcSpotLight(spotLight,norm,FragPos,viewDir);
	FragColor = vec4(result, 1.0); //* texture(ourTexture,TexCoords);
}