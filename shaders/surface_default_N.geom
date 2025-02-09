#version 330 core
layout (TRIANGLES) in;
layout (line_strip, max_vertices=6) out;

in VS_OUT{
    vec3 N;
} vs_out[];

uniform mat4 projection;

const float SCALE = 0.1;

void GenerateLine(vec4 T,vec3 N){
    gl_Position = projection * T;
    EmitVertex();
    gl_Position = projection *(T+vec4(N,0.0)*SCALE);
    EmitVertex();
    EndPrimitive();
}

void main(){
    GenerateLine(gl_in[0].gl_Position,vs_out[0].N);
    GenerateLine(gl_in[1].gl_Position,vs_out[1].N);
    GenerateLine(gl_in[2].gl_Position,vs_out[2].N);
}