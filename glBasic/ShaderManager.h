#pragma once

#include <unordered_map>

#include "Shader.h"

enum class ShaderType{
    SHADER_PHONE,

    SHADER_MESH_DEFAULT,
    SHADER_MESH_DEFAULT_NORMAL,
    SHADER_MESH_DEFAULT_REGION,

    SHADER_CURVE_DEFAULT,
    SHADER_CURVE_DEFAULT_VECTOR,
};


class ShaderManager {
public:
    static ShaderManager& getInstance(){
        static ShaderManager instance;
        return instance;
    }
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    std::shared_ptr<Shader> getShader(ShaderType type){
        if(Shaders.find(type)!=Shaders.end())return Shaders[type];
        return Shaders[type]=createShader(type);
    }

    static void cleanUp(){
        for(auto& [type,shader]:getInstance().Shaders){
            shader->release();
        }
        getInstance().Shaders.clear();
    }

private:
    ShaderManager(){}
    std::unordered_map<ShaderType, std::shared_ptr<Shader>> Shaders;
    static std::shared_ptr<Shader> createShader(ShaderType type);
};