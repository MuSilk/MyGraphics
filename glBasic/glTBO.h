#pragma once

#include <stdint.h>
#include <glad/glad.h>

class glTBO{
private:
    uint32_t TBO,TEX;
    GLenum type;
public:
    void init(void* data,size_t size,GLenum type);
    void bind(int id);
    void release();
};