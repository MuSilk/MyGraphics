#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <array>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <surface/Mesh.h>
#include <glBasic/Camera.h>
#include <object/surface.h>
#include <common/IdManager.h>

enum class ObjectType{
    COMMON,
    PHONE,
    LIGHT,
    ObjectGroup
};

class Scene;

class RenderObject {
public:
    uint32_t id;
    std::string name;
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    DataObject* dataSurface;
    DataObject* clickRegion;
    bool selected=false;
    std::function<void(RenderObject&)> update=[](RenderObject& obj){};
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
    virtual ObjectType objectType() const{return ObjectType::COMMON;}
private:
    typedef std::array<glm::vec3,3> Triangle;
    static bool intersect_triangle(Triangle tri,glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t);
};

class Light:public RenderObject{
public:
    virtual ObjectType objectType() const override{return ObjectType::LIGHT;}

    glm::vec3 color=glm::vec3(1.0f);
    float constant=1.0f,linear=0.045f,quadratic=0.0075f;    

    void initShadowMap(uint32_t width,uint32_t height);
    void generateShadowMap(shared_ptr<Scene> scene,float near,float far);
private:
    uint32_t shadowCubeMap,shadowMapFBO,shadowMapWidth,shadowMapHeight;
};

class PhoneObject:public RenderObject{
public:
    shared_ptr<Surface> diffuse,specular;
    float shininess;
    virtual ObjectType objectType() const override{return ObjectType::PHONE;}

    static PhoneObject evalMeshObject(
        glm::vec3 pos,DataObject* datasource,
        Camera& camera,uint32_t& Width,uint32_t& Height);

    void defaultrender_phone(
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 viewPos,
        Light* light,
        glm::vec3 ambientColor=glm::vec3(0.5f)
    );
};

class ObjectGroup:public RenderObject{
public:
    virtual ObjectType objectType() const override{return ObjectType::ObjectGroup;}
    std::map<std::uint32_t,shared_ptr<RenderObject>> Objects;
    std::map<std::uint32_t,shared_ptr<Light>> Lights;
    void addObject(const RenderObject& obj);
    void delObject(uint32_t id);
private:
    IdManager ObjectsIdManager;
};