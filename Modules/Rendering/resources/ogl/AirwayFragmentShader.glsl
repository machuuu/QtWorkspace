#version 460 core

out vec4 FragColor;

struct Light {
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Material {
	vec3 frontcolor;
	vec3 backcolor;
	float shininess;
};

in vec3 v_FragPos;
in vec3 v_Normal;

uniform Material object;
uniform Light light;
uniform vec3 viewPos;

//! [0]
void main()
{
	// directional vectors
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(light.position - v_FragPos);
	vec3 viewDir = normalize(viewPos - v_FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	// attenuation
	float dist = length(light.position - v_FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	/* Front Material */
	// ambient lighting
	vec3 frontAmbient = light.ambient;
	
	// diffuse lighting	
	float frontDiff = max(dot(norm, lightDir), 0.0);
	vec3 frontDiffuse = light.diffuse * frontDiff;

	// specular lighting
	vec3 frontSpecular;
	float frontSpec;
	if (dot(norm, lightDir) < 0.0)
	{
		frontSpecular = vec3(0.0,0.0,0.0);
	}
	else
	{
		frontSpec = pow(max(dot(viewDir, reflectDir), 0.0), object.shininess);
		frontSpecular = light.specular * frontSpec;
	}

	// total front lighting
	vec3 frontLight = (frontAmbient + frontDiffuse + frontSpecular) * attenuation * object.frontcolor;
	
	/* Back Material */
	// ambient lighting
	vec3 backAmbient = light.ambient;

	// diffuse lighting
	float backDiff = max(dot(-norm, lightDir), 0.0);
	vec3 backDiffuse = light.diffuse * backDiff;

	// specular lighting
	vec3 backSpecular;
	float backSpec;
	if (dot(-norm, lightDir) < 0.0)
	{
		backSpecular = vec3(0.0,0.0,0.0);
	}
	else
	{
		backSpec = pow(max(dot(viewDir, reflectDir), 0.0), object.shininess);
		backSpecular = light.specular * backSpec;
	}

	// total back lighting
	vec3 backLight = (backAmbient + backDiffuse + backSpecular) * attenuation * object.backcolor;

	// Two-sided surface
	if (gl_FrontFacing)
	{
		FragColor = vec4(frontLight, 1.0);
	}
	else
	{
		FragColor = vec4(backLight, 1.0);
	}
}
//! [0]

