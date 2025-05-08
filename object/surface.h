#pragma once
#include <glm/glm.hpp>
#include <string>

#include <glBasic/Texture.h>

enum class SurfaceType{
    NONE,
    COLOR,
    TEXTURE
};

std::string to_string(SurfaceType type);

class Surface {
public:
    virtual SurfaceType surfaceType(){return SurfaceType::NONE;}
};

class SurfaceColor:public Surface{
public:
    SurfaceColor(){}
    SurfaceColor(glm::vec3 color):color(color){}
    
    glm::vec3 color=glm::vec3(1.0f);
    virtual SurfaceType surfaceType() override{return SurfaceType::COLOR;}
};

class SurfaceTexture:public Surface{
public:
    string textureName;
    shared_ptr<Texture2D> texture;
    virtual SurfaceType surfaceType() override{return SurfaceType::TEXTURE;}
};