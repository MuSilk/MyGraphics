#version 330 core

struct Light {
    vec3 position;
    vec3 color;
    float constant, linear, quadratic;
};

struct Material {
    vec4 diffuse;
    sampler2D diffuse_texture;
    bool use_diffuse_texture;

    vec4 specular;
    sampler2D specular_texture;
    bool use_specular_texture;

    float shininess;
};

in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform vec3 ambientColor;
out vec4 outColor;

void main(){
    vec4 materialDiffuse=material.diffuse;
    if(material.use_diffuse_texture){
        materialDiffuse=texture(material.diffuse_texture,aTexCoord);
    }
    vec4 materialSpecular=material.specular;
    if(material.use_specular_texture){
        materialSpecular=texture(material.specular_texture,aTexCoord);
    }

    vec4 ambient=materialDiffuse*vec4(ambientColor,1.0);

    vec3 norm=normalize(aNormal);
    vec3 lightDir=normalize(light.position-aPos);
    float diff=max(dot(norm,lightDir),0.0);
    vec4 diffuse=materialDiffuse*diff*vec4(light.color,1.0);

    vec3 viewDir=normalize(viewPos-aPos);
    vec3 reflectDir=reflect(-lightDir,norm);
    float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
    vec4 specular=materialSpecular*spec*vec4(light.color,1.0);

    float dist = length(light.position - aPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    outColor = ambient+(diffuse+specular)*attenuation;
}