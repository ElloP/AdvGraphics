#version 420 core

layout (binding = 0) uniform sampler2D diffuseMap;
layout (binding = 1) uniform sampler2D specularMap;

out vec4 FragColor;

struct Material
{
	vec3 ambient; // not using
	vec3 diffuse; // not using
	vec3 specular; // not using
	float shininess;
};
struct Light
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;
uniform Material material;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);

	float diff = max(dot(lightDir,norm),0.0);
	vec3 diffuse = diff * light.diffuse * vec3(texture(diffuseMap,  TexCoords));

	vec3 ambient = light.ambient * vec3(texture(diffuseMap,TexCoords));

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir,norm);


	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(specularMap, TexCoords)) * spec * light.specular;  

	vec3 result = (ambient + diffuse + specular);
	FragColor = vec4(result, 1.0); //* texture(ourTexture,TexCoords);
}