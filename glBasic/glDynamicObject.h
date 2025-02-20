#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class glDynamicObject{
protected:
    uint32_t _size;
    uint32_t VAO,VBO,EBO;
    bool useEBO;
    size_t sum;
public:
    void init(size_t size,std::initializer_list<uint32_t> partition);
    void init(size_t size,std::initializer_list<uint32_t> partition,std::vector<uint32_t> indices);
    void update(const float* const vertices);
    void render(GLenum mode);
    void release();

};