#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <array>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <surface/Surface.h>

class Material {
public:
    virtual std::string MaterialType(){return "None";}
};

class MaterialColor:public Material{
public:
    glm::vec3 color=glm::vec3(1.0f);
    virtual std::string MaterialType() override{return "SingleColor";}
};

class RenderObject {
public:
    uint32_t id;
    std::string name;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    DataObject* surface;
    DataObject* clickRegion;
    Material* material;
    bool selected=false;
    std::function<void(const RenderObject&)> render;
    std::function<void(const RenderObject&)> render_margin;
    glm::mat4 GetScaleMatrix() const{
        return glm::scale(glm::mat4(1.0f),scale);
    }
    glm::mat4 GetTranslateMatrix() const{
        return glm::translate(glm::mat4(1.0f),position);
    }
    glm::mat4 GetRotateMatrix() const{
        return glm::mat4_cast(glm::normalize(rotation));
    }
    glm::mat4 GetModelMatrix() const{
        return GetTranslateMatrix()*GetRotateMatrix()*GetScaleMatrix();
    }
    bool intersect(glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t);
    static void render_select(const std::map<std::string,RenderObject>& objects);
private:
    typedef std::array<glm::vec3,3> Triangle;
    static bool intersect_triangle(Triangle tri,glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t);
};

class Light{
public:
    glm::vec3 position;
    glm::vec3 color; 

    std::function<void(const RenderObject&)> render;
};