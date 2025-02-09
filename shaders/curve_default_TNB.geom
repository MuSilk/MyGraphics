#version 330 core
layout (points) in;
layout (line_strip, max_vertices=6) out;

in VS_OUT{
    vec3 T,N,B;
} vs_out[];

uniform mat4 projection;

const float SCALE = 0.1;

out vec4 frag_color;

void GenerateLine(vec3 N,vec4 C){
    gl_Position = projection * gl_in[0].gl_Position;
    frag_color = C;
    EmitVertex();
    gl_Position = projection *(gl_in[0].gl_Position+vec4(N,0.0)*SCALE);
    frag_color = C;
    EmitVertex();
    EndPrimitive();
}

void main(){
    GenerateLine(vs_out[0].T,vec4(1.0,0.0,0.0,1.0));
    GenerateLine(vs_out[0].N,vec4(0.0,1.0,0.0,1.0));
    GenerateLine(vs_out[0].B,vec4(0.0,0.0,1.0,1.0));
}