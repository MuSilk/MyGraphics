#include "RayTrace.h"

#include <surface/Surface.h>
#include <raytrace/BVHTree.h>
Surface model;
SurfaceTBO modelTBO;
BVHTreeTBO treeTBO;


void RayTrace::init(int width,int height){
    Screen=ScreenVT::evalQuad();
    Screen.init();

    Buffer[0].init(width,height);
    Buffer[1].init(width,height);

    screenShader=defaultShader_screen();
    rayTraceShader=defaultShader_rayTrace();

    model=Surface::evalSphere(0.25,32);

    modelTBO.init(model);
    treeTBO.init(model);
}

static float GetRandamSeed() {
	return 674764.0f * ((float)rand() / (RAND_MAX + 1.0f) + 1);
}

void RayTrace::initrender(){
    LoopNum=1;
}
void RayTrace::render(const Camera& camera,int width,int height){
    {
        Shader shader=rayTraceShader;
        shader.use();
        Buffer[currentBuffer^1].useAsTexture();
        shader.setInt("historyTexture",0);
        shader.set1f("randOrigin",GetRandamSeed());
        shader.setInt("LoopNum",LoopNum);

        shader.setVec3("camera.camPos",camera.Position);
        shader.setVec3("camera.front",camera.Front);
        shader.setVec3("camera.right",camera.Right);
        shader.setVec3("camera.up",camera.Up);
        shader.setVec2("camera.grid",camera.GetGrid(width,height));

        shader.setInt("texMeshVertex",1);
        shader.setInt("texMeshFaceIndex",2);
        shader.setInt("meshFaceNum",modelTBO.getFaceNum());
        shader.setInt("bvhTreePtr",3);
        shader.setInt("bvhTreeBounds",4);
        modelTBO.use();
        treeTBO.use();
        
        float floorHfW = 1.0, upBias = -0.22;
        shader.setVec3("triFloor[0].v[0]", glm::vec3(-floorHfW, upBias, floorHfW));
        shader.setVec3("triFloor[0].v[1]", glm::vec3(-floorHfW, upBias, -floorHfW));
        shader.setVec3("triFloor[0].v[2]", glm::vec3(floorHfW, upBias, floorHfW));
        shader.setVec3("triFloor[1].v[0]", glm::vec3(floorHfW, upBias, floorHfW));
        shader.setVec3("triFloor[1].v[1]", glm::vec3(-floorHfW, upBias, -floorHfW));
        shader.setVec3("triFloor[1].v[2]", glm::vec3(floorHfW, upBias, -floorHfW));
        shader.setVec3("triFloor[0].n[0]", glm::vec3(0, 1.0, 0));
        shader.setVec3("triFloor[0].n[1]", glm::vec3(0, 1.0, 0));
        shader.setVec3("triFloor[0].n[2]", glm::vec3(0, 1.0, 0));
        shader.setVec3("triFloor[1].n[0]", glm::vec3(0, 1.0, 0));
        shader.setVec3("triFloor[1].n[1]", glm::vec3(0, 1.0, 0));
        shader.setVec3("triFloor[1].n[2]", glm::vec3(0, 1.0, 0));


        Buffer[currentBuffer].use();
        Screen.render();
        Buffer[currentBuffer].unuse();

    }
    {
        Shader shader=screenShader;
        shader.use();
        shader.setInt("screenTexture",0);
        Buffer[currentBuffer].useAsTexture();
        Screen.render();
    }
    currentBuffer^=1;
    LoopNum+=1;
}

void RayTrace::release(){
    Screen.release();
    Buffer[0].release();
    Buffer[1].release();
    rayTraceShader.release();
    screenShader.release();
}

#include <shaders/rc.h>
Shader RayTrace::defaultShader_rayTrace(){
    static Shader shader;
    shader.compile((char*)RAY_TRACE_VERT_DATA,nullptr,(char*)RAY_TRACE_FRAG_DATA);
    return shader;
}

Shader RayTrace::defaultShader_screen(){
    static Shader shader;

    const char vert[]=
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoords;\n"

        "out vec2 TexCoords;\n"

        "void main(){\n"
	    "    TexCoords = aTexCoords;\n"
	    "    gl_Position = vec4(aPos,0.0, 1.0);\n"
        "}\0";

    const char frag[]=
        "#version 330 core\n"
        "out vec4 FragColor;\n"
    
        "in vec2 TexCoords;\n"
    
        "uniform sampler2D screenTexture;\n"
    
        "void main() {\n"
        "   FragColor=vec4(texture(screenTexture,TexCoords).rgb,1.0);\n"
        "}\0";

    shader.compile(vert,nullptr,frag);
    return shader;
}