#pragma once

#include <glm/glm.hpp>

#include <glBasic/glObject.h>

struct PointVT{
    glm::vec2 V;
    glm::vec2 T;
};

class ScreenVT:public glObject{
public:
    std::vector<PointVT> vertices;
    std::vector<uint32_t> indices;

    void init();
    void render();
    static ScreenVT evalQuad();
};

class ScreenVTFBO{
private:
    uint32_t FBO;
    uint32_t TEX;
public:
    void init(int width,int height);
    void use();
    void unuse();
    void useAsTexture();
    void release();
};