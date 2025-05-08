#pragma once

#include <glBasic/glObject.h>
#include <glBasic/glTBO.h>
#include <object/DataObject.h>
#include <object/object.h>
#include "Curve.h"

#include "object/surface.h"
#include <glBasic/ShaderManager.h>

struct MeshPoint{
    glm::vec3 V,N;
    static const size_t SIZE=6; 
    constexpr static std::initializer_list<uint32_t> PARTITION={3,3};
    constexpr static char* vertexType="MeshPoint";
};

struct TexturedMeshPoint{
    glm::vec3 V,N;
    glm::vec2 T;
    static const size_t SIZE=8;
    constexpr static std::initializer_list<uint32_t> PARTITION={3,3,2};
    constexpr static char* vertexType="TexturedMeshPoint";
};

template<class T>
class Mesh:public glObject,public DataObject{
public:
    Mesh(){useIndex=true;geometry="Mesh";vertexType=T::vertexType;};
    virtual DataType dataType() const override {return DataType::MESH;}
    void init(){
        glObject::init(reinterpret_cast<float*>(vertices.data()),vertices.size()/T::SIZE,T::PARTITION,indices);
    }
    T& getPoint(size_t index) const{
        return *(T*)(vertices.data()+index*T::SIZE);
    }
    size_t size() const{return vertices.size()/T::SIZE;};
    T& front() const{return getPoint(0);}
    T& back() const{return getPoint(size()-1);}
    T& operator[](size_t index) const{return getPoint(index);}

    void defaultrender(
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 viewPos,
        glm::vec3 lightDir=glm::normalize(glm::vec3(-1.0f,1.0f,0.0f)),    
        glm::vec3 materialDiffuse=glm::vec3(0.4f),
        glm::vec3 materialSpecular=glm::vec3(0.0f),
        float materialShininess=20.0f,
        glm::vec3 lightColor=glm::vec3(1.0f),
        glm::vec3 ambientColor=glm::vec3(0.5f)
    ){
        auto shader=ShaderManager::getInstance().getShader(ShaderType::SHADER_MESH_DEFAULT);
        shader->use();

        shader->setMat4("model",model);
        shader->setMat4("view",view);
        shader->setMat4("projection",projection);

        shader->setVec3("viewPos",viewPos);
        shader->setVec3("material.diffuse",materialDiffuse);
        shader->setVec3("material.specular",materialSpecular);
        shader->set1f("material.shininess",20.0f);
        shader->setVec3("light.direction",lightDir);
        shader->setVec3("light.color",lightColor);
        shader->setVec3("ambientColor",ambientColor);

        render(GL_TRIANGLES);
    }

    void defaultrender_T(
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 viewPos,
        glm::vec3 lightDir=glm::normalize(glm::vec3(-1.0f,1.0f,0.0f)),
        glm::vec3 materialDiffuse=glm::vec3(0.4f),
        glm::vec3 materialSpecular=glm::vec3(0.0f),
        float materialShininess=20.0f,
        glm::vec3 lightColor=glm::vec3(1.0f),
        glm::vec3 ambientColor=glm::vec3(0.5f)
    ){
        defaultrender(model,view,projection,viewPos,lightDir,materialDiffuse,materialSpecular,materialShininess,lightColor,ambientColor);
	
        auto shader=ShaderManager::getInstance().getShader(ShaderType::SHADER_MESH_DEFAULT_NORMAL);
        shader->use();
        shader->setMat4("model",model);
        shader->setMat4("view",view);
        shader->setMat4("projection",projection);
        glObject::render(GL_TRIANGLES);
    }

    void defaultrender_region(
        glm::mat4 mvp,
        glm::vec3 color
    ){
        auto shader=ShaderManager::getInstance().getShader(ShaderType::SHADER_MESH_DEFAULT_REGION);
        shader->use();
        shader->setMat4("mvp",mvp);
        shader->setVec3("color",color);
        render(GL_TRIANGLES);
    }
    
    static std::shared_ptr<Mesh<MeshPoint>> evalCircle(float radius,uint32_t steps);
    static std::shared_ptr<Mesh<MeshPoint>> evalSphere(float radius,uint32_t steps);
    static std::shared_ptr<Mesh<MeshPoint>> evalQuad();
	static std::shared_ptr<Mesh<T>> evalCube();
	static std::shared_ptr<Mesh<MeshPoint>> evalSweepSurf(const Curve& profile, unsigned steps);
	static std::shared_ptr<Mesh<MeshPoint>> evalCylinder(const Curve& profile, const Curve& sweep);
	static std::shared_ptr<Mesh<MeshPoint>> evalModel(const char* filename);
};

class MeshTBO{
    glTBO vertices,indices;
    uint32_t FaceNum;

    public:
    void init(const Mesh<MeshPoint>& mesh);
    void use();
    void release();
    uint32_t getFaceNum();
};