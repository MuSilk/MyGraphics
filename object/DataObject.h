#pragma once

#include <vector>

class DataObject {
public:
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    bool useIndex;

    float* getAttribute(uint32_t index, uint32_t type_float_cnt, uint32_t offset_float_cnt){
        return &vertices[index*type_float_cnt+offset_float_cnt];
    }
};