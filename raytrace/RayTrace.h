#pragma once

#include <map>

#include <glBasic/Shader.h>
#include <glBasic/Camera.h>
#include <surface/ScreenVT.h>

class RayTrace
{
private:
    int currentBuffer=0;
    int LoopNum=1;
    ScreenVT Screen;
    ScreenVTFBO Buffer[2];
    Shader screenShader,rayTraceShader;
public:
    void init(int width,int height); 
    void initrender();
    void render(const Camera& camera,int width,int height);
    void release();
private:
    static Shader defaultShader_rayTrace();
    static Shader defaultShader_screen();
};
