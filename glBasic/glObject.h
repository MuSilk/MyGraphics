#pragma once

#include <vector>

#include "Shader.h"

class glObject{
protected:
    uint32_t _size;
    uint32_t VAO,VBO,EBO;
    bool useEBO;
public:
    void init(const float* const vertices,size_t size,std::initializer_list<uint32_t> partition);
    void init(const float* const vertices,size_t size,std::initializer_list<uint32_t> partition,const std::vector<uint32_t>& indices);

    void render(GLenum mode);
    void release();
};
