#version 330 core

struct Light {
    vec3 position;
    vec3 color;
    float constant, linear, quadratic;
};

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 aPos;
in vec3 aNormal;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform vec3 ambientColor;
out vec4 outColor;

void main(){
    vec3 ambient=material.diffuse*ambientColor;

    vec3 norm=normalize(aNormal);
    vec3 lightDir=normalize(light.position-aPos);
    float diff=max(dot(norm,lightDir),0.0);
    vec3 diffuse=material.diffuse*diff*light.color;

    vec3 viewDir=normalize(viewPos-aPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=material.specular*spec*light.color;

    float dist = length(light.position - aPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 result=ambient+(diffuse+specular)*attenuation;
    outColor = vec4(result,1.0);
}