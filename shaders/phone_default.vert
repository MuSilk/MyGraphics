#version 330 core

layout(location = 0) in vec3 V;
layout(location = 1) in vec3 N;
layout(location = 2) in vec2 T;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 aPos;
out vec3 aNormal;
out vec2 aTexCoord;

void main(){
    gl_Position = projection * view * model * vec4(V,1.0);
    aPos = vec3(model*vec4(V,1.0));
    aNormal = mat3(transpose(inverse(model))) * N;

    aTexCoord = T;
}