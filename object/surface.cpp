#include "surface.h"

std::string to_string(SurfaceType type){
    switch(type){
        case SurfaceType::NONE:return "None";break;
        case SurfaceType::COLOR:return "Color";break;
        case SurfaceType::TEXTURE:return "Texture";break;
    }
}