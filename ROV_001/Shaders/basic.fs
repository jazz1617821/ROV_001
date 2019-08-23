/*
 * ROV_001 
 * Copyright (C) 2019 ROV Simulator
 * National Taiwan Ocean University 
 * Department of Computer Science and Engineering
 * Lab 503 Chieh-Shih Chou, Shyh-Kuang Ueng
 * Copyright (C) 2019
 */

// Fragment shader:
// ================
#version 430 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
}; 
/* Note: because we now use a material struct again you want to change your
mesh class to bind all the textures using material.texture_diffuseN instead of
texture_diffuseN. */

#define MAX_POINT_LIGHTS 10
#define MAX_DIRECT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirectLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff_angle;
    float exponent;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPosition;
in vec3 Normal;
in vec2 TexCoords;

in vec3 fragAmbient;
in vec3 fragDiffuse;
in vec3 fragSpecular;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec4 foggyColor;
uniform int num_PointLight;
uniform int num_DirectLight;
uniform int num_SpotLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirectLight directLights[MAX_DIRECT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

// Function prototypes
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirectLight(DirectLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcFoggyColor(vec3 Color,float d,vec4 foggyColor);

void main()
{    
    vec3 result = vec3(0.0f);
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 norm = normalize(Normal);
    
    // point light
    for(int i = 0; i < num_PointLight; i++){
        result += CalcPointLight(pointLights[i], material, norm, fragPosition, viewDir);
    }
    // direct light
    for(int i = 0; i < num_DirectLight; i++){
        result += CalcDirectLight(directLights[i], material, norm, fragPosition, viewDir);
    }
    // spot light
    for(int i = 0; i < num_SpotLight; i++){
        result += CalcSpotLight(spotLights[i], material, norm, fragPosition, viewDir);
    }

    float d = distance(viewPos, fragPosition);
    
    vec4 maped_foggyColor;
    if(fragPosition.y < 0){
        float scale = abs(fragPosition.y) / 20.0;
        if(scale > 1){
            scale = 1;
        }
        maped_foggyColor = vec4(foggyColor.xyz, foggyColor.a * scale);
    }else{
        maped_foggyColor = vec4(foggyColor.xyz, 0.0);
    }

    result = CalcFoggyColor(result, d, maped_foggyColor);
    
    FragColor = vec4(result, 1.0f);
}


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);  

    // Attenuation
    float dist = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * dist +light.quadratic * ( dist * dist ));
    
    // Combine results
    vec3 ambient = light.ambient * max(vec3(texture(mat.texture_diffuse1, TexCoords)),fragAmbient);
    vec3 diffuse = light.diffuse * diff * max(vec3(texture(mat.texture_diffuse1, TexCoords)),fragDiffuse);
    vec3 specular = light.specular * spec * max(vec3(texture(mat.texture_specular1, TexCoords)),fragSpecular);
    
    vec3 Color = (ambient + diffuse + specular) * attenuation;

    return Color;
}

// Calculates the color when using a point light.
vec3 CalcDirectLight(DirectLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);  

    // length from seaplane y = 0.0
    float dist = abs(0.0 - fragPos.y);
    float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * ( dist * dist ));

    // Combine results
    vec3 ambient = light.ambient * max(vec3(texture(mat.texture_diffuse1, TexCoords)),fragAmbient);
    vec3 diffuse = light.diffuse * diff * max(vec3(texture(mat.texture_diffuse1, TexCoords)),fragDiffuse);
    vec3 specular = light.specular * spec * max(vec3(texture(mat.texture_specular1, TexCoords)),fragSpecular);
    
    vec3 Color = (ambient + diffuse + specular) * attenuation;

    return Color;
}

// Calculates the color when using a point light.
vec3 CalcSpotLight(SpotLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float spotDot;              // cosine of angle between spotlight and VP
	float spotAttenuation;      // spotlight attenuation factor;

	// Compute vector from surface to light position
	vec3 VP = normalize(vec3 (light.position) - fragPos);

    // See if point on surface is inside cone of illumination
	spotDot = dot (-VP, normalize (light.direction));

	if (spotDot < cos(radians(light.cutoff_angle)))
	{
		spotAttenuation = 0.0f;
	}
	else
	{
		// we are going to ramp from the outer cone value to the inner using
		// smoothstep to create a smooth value for the falloff
		float spotValue = smoothstep( cos(radians(light.cutoff_angle)), cos(radians(light.cutoff_angle * 0.8f)), spotDot);
		spotAttenuation = pow (spotValue, light.exponent);
	}

    // Diffuse shading
    float diff = max(dot(normal, VP), 0.0);
    // Specular shading
    vec3 reflectDir = normalize(reflect(-VP, normal)); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess);  

     // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance +light.quadratic * ( distance * distance ));

	// combine the light / material values
    vec3 ambient = light.ambient * max(vec3(texture(mat.texture_diffuse1, TexCoords)),fragAmbient) * spotAttenuation;
    vec3 diffuse = light.diffuse * diff * max(vec3(texture(mat.texture_diffuse1, TexCoords)),fragDiffuse) * spotAttenuation;
    vec3 specular = light.specular * spec * max(vec3(texture(mat.texture_specular1, TexCoords)),fragSpecular) * spotAttenuation;

	return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcFoggyColor(vec3 Color,float d,vec4 foggyColor){
    float f = exp(-1.0 * foggyColor.a * d);
	vec3 result = f * Color.rgb + (1.0 - f) * foggyColor.rgb;
	return result;
}