#include "surface.h"

std::string to_string(SurfaceType type){
    switch(type){
        case SurfaceType::NONE:return "NONE";break;
        case SurfaceType::COLOR:return "COLOR";break;
        case SurfaceType::TEXTURE:return "TEXTURE";break;
    }
}