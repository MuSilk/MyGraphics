#include "ShaderManager.h"

#include <shaders/rc.h>
std::shared_ptr<Shader> ShaderManager::createShader(ShaderType type){
    std::shared_ptr<Shader> shader=std::make_shared<Shader>();
    switch(type){
        case ShaderType::SHADER_PHONE:{  
            shader->compile((char*)PHONE_DEFAULT_VERT_DATA,nullptr,(char*)PHONE_DEFAULT_FRAG_DATA);
            return shader;
        }
        case ShaderType::SHADER_MESH_DEFAULT:{
            shader->compile((char*)SURFACE_DEFAULT_VERT_DATA,nullptr,(char*)SURFACE_DEFAULT_FRAG_DATA);
            return shader;
        }
        case ShaderType::SHADER_MESH_DEFAULT_NORMAL:{
            shader->compile(
                (char*)SURFACE_DEFAULT_N_VERT_DATA,
                (char*)SURFACE_DEFAULT_N_GEOM_DATA,
                (char*)SURFACE_DEFAULT_N_FRAG_DATA
            );
            return shader;
        }
        case ShaderType::SHADER_MESH_DEFAULT_REGION:{
            const char* vert =
            "#version 330 core\n"
            "layout (location = 0) in vec3 V;\n"
            "layout (location = 1) in vec3 N;\n"
            "uniform mat4 mvp;"
            "uniform vec3 color;"
            "layout (location = 0) out vec3 fragColor;\n"
            "void main(){\n"
            "    gl_Position = mvp*vec4(V,1.0);\n"
            "    fragColor = color;\n"
            "}\0";

            const char* frag =
                "#version 330 core\n"
                "layout(location=0) in vec3 fragColor;\n"
                "out vec4 FragColor;\n"
                "void main()\n"
                "{\n"
                "    FragColor = vec4(fragColor, 1.0f);\n"
                "}\0";

            shader->compile((char*)vert,nullptr,(char*)frag);
            return shader;
        }
        case ShaderType::SHADER_CURVE_DEFAULT:{
            const char* vert =
            "#version 330 core\n"
            "layout (location = 0) in vec3 V;\n"
            "layout (location = 1) in vec3 T;\n"
            "layout (location = 2) in vec3 N;\n"
            "layout (location = 3) in vec3 B;\n"

            "uniform mat4 mvp;\n"

            "void main(){\n"
            "	gl_Position = mvp*vec4(V,1.0);\n"
            "}\0";

            const char* frag =
            "#version 330 core\n"

            "out vec4 outColor;\n"
            "uniform vec3 color;\n"
            "void main(){\n"
            "	outColor = vec4(color,1.0f);\n"
            "}\0";

            shader->compile(
                (char*)vert,
                (char*)nullptr,
                (char*)frag
            );
            return shader;
        }
        case ShaderType::SHADER_CURVE_DEFAULT_VECTOR:{
            shader->compile(
                (char*)CURVE_DEFAULT_TNB_VERT_DATA,
                (char*)CURVE_DEFAULT_TNB_GEOM_DATA,
                (char*)CURVE_DEFAULT_TNB_FRAG_DATA
            );
            return shader;
        }
        case ShaderType::SHADER_SHADOW_DEPTH_MAP:{
            const char shadowvert[] = {
                "#version 330 core\n"
        
                "layout(location = 0) in vec3 aPos;"
        
                "uniform mat4 model;"
        
                "void main() {"
                "    gl_Position =  model * vec4(aPos, 1.0);"
                "}"
            };
            const char shadowgeog[] = {
                "#version 330 core\n"
                "layout (triangles) in;"
                "layout (triangle_strip,max_vertices=18)out;"
                "uniform mat4 shadowMatrices[6];"
                "out vec4 FragPos;"
        
                "void main(){"
                "    for(int face=0;face<6;face++){"
                "	     gl_Layer=face;"
                "		 for(int i=0;i<3;i++){"
                "            FragPos=gl_in[i].gl_Position;"
                "            gl_Position=shadowMatrices[face]*FragPos;"
                "            EmitVertex();"
                "        }"
                "        EndPrimitive();"
                "    }"
                "}"
            };
            const char shadowfrag[] = {
                "#version 330 core\n"
                "in vec4 FragPos;"
        
                "uniform vec3 lightPos;"
                "uniform float far_plane;"
                "void main(){"
                "    float lightDistance=length(FragPos.xyz-lightPos);"
                "    lightDistance=lightDistance/far_plane;"
                "    gl_FragDepth=lightDistance;"
                "}"
            };
            shader->compile(shadowvert,shadowgeog,shadowfrag);
            return shader;
        }
    }
    return nullptr;
}