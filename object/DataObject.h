#pragma once

#include <vector>
#include <string>
#include <common/AttrManager.h>

enum class DataType{
    UNDEFINED,
    MESH,
    CURVE
};
std::string to_string(DataType type);

class DataObject {
public:
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    bool useIndex;
    std::string geometry="Object";
    std::string vertexType="None";
    std::string name;
    AttrManager attrs;
    virtual DataType dataType() const {return DataType::UNDEFINED;}
    float* getAttribute(uint32_t index, uint32_t type_float_cnt, uint32_t offset_float_cnt){
        return &vertices[index*type_float_cnt+offset_float_cnt];
    }
};