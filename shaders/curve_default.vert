#version 330 core

layout (location = 0) in vec3 V;
layout (location = 1) in vec3 T;
layout (location = 2) in vec3 N;
layout (location = 3) in vec3 B;

uniform mat4 mvp;

void main(){
    gl_Position = mvp*vec4(V,1.0);
}