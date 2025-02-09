#version 330 core

in vec3 aPos;
in vec3 aNormal;

struct Material{
    vec3 diffuse,specular;
    float shininess;
};

struct Light{
    vec3 direction,color;
};

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform vec3 ambientColor;
out vec4 outColor;

void main(){
    vec3 ambient=material.diffuse*ambientColor;

    vec3 norm=normalize(aNormal);
    vec3 lightDir=light.direction;
    float diff=max(dot(norm,lightDir),0.0);
    vec3 diffuse=material.diffuse*diff*light.color;

    vec3 viewDir=normalize(viewPos-aPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec3 specular=material.specular*spec*light.color;

    vec3 result=ambient+diffuse+specular;
    outColor = vec4(result,1.0);
}