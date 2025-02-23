#pragma once

#include <vector>
#include <string>

enum class DataType{
    UNDEFINED,
    MESH,
    CURVE
};

class DataObject {
public:
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    bool useIndex;
    std::string name="Object";

    DataType type=DataType::UNDEFINED;

    float* getAttribute(uint32_t index, uint32_t type_float_cnt, uint32_t offset_float_cnt){
        return &vertices[index*type_float_cnt+offset_float_cnt];
    }
};