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


class ShaderManagerv1 {
public:
    static ShaderManagerv1& getInstance(){
        static ShaderManagerv1 instance;
        return instance;
    }
    ShaderManagerv1(const ShaderManagerv1&) = delete;
    ShaderManagerv1& operator=(const ShaderManagerv1&) = delete;

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
    ShaderManagerv1(){}
    std::unordered_map<ShaderType, std::shared_ptr<Shader>> Shaders;
    static std::shared_ptr<Shader> createShader(ShaderType type);
};