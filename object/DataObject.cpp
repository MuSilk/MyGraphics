#include "DataObject.h"

std::string to_string(DataType type){
    static const char* type_names[] = {
        "Undefined",
        "Mesh",
        "Curve"
    };
    return type_names[static_cast<int>(type)];
}