#version 330 core

layout (location = 0) in vec3 V;
layout (location = 1) in vec3 T;
layout (location = 2) in vec3 N;
layout (location = 3) in vec3 B;

uniform mat4 model;
uniform mat4 view;

out VS_OUT{
    vec3 T,N,B;
} vs_out;

void main(){
    gl_Position = view * model * vec4(V,1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.T = normalize(normalMatrix * T);
    vs_out.N = normalize(normalMatrix * N);
    vs_out.B = normalize(normalMatrix * B);
}