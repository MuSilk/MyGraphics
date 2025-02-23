#pragma once

#include <glBasic/glObject.h>
#include <glBasic/glTBO.h>
#include <object/DataObject.h>
#include <object/object.h>
#include "Curve.h"

struct MeshPoint{
    glm::vec3 V,N;
    static const size_t SIZE=6; 
    constexpr static std::initializer_list<uint32_t> PARTITION={3,3};
};

class Mesh:public glObject,public DataObject{
public:
    Mesh(){useIndex=true;type=DataType::MESH;name="Mesh";}
    void init();

    MeshPoint& getPoint(size_t index) const;
    size_t size() const{return vertices.size()/MeshPoint::SIZE;};
    MeshPoint& front() const{return getPoint(0);}
    MeshPoint& back() const{return getPoint(size()-1);}
    MeshPoint& operator[](size_t index) const{return getPoint(index);}

    void defaultrender(
        std::map<std::string,Shader>& shaderManager,
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 viewPos,
        glm::vec3 lightDir=glm::normalize(glm::vec3(-1.0f,1.0f,0.0f)),
        glm::vec3 materialDiffuse=glm::vec3(0.4f),
        glm::vec3 materialSpecular=glm::vec3(0.0f),
        float materialShininess=20.0f,
        glm::vec3 lightColor=glm::vec3(1.0f),
        glm::vec3 ambientColor=glm::vec3(0.5f)
    );

    void defaultrender_T(
        std::map<std::string,Shader>& shaderManager,
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 viewPos,
        glm::vec3 lightDir=glm::normalize(glm::vec3(-1.0f,1.0f,0.0f)),
        glm::vec3 materialDiffuse=glm::vec3(0.4f),
        glm::vec3 materialSpecular=glm::vec3(0.0f),
        float materialShininess=20.0f,
        glm::vec3 lightColor=glm::vec3(1.0f),
        glm::vec3 ambientColor=glm::vec3(0.5f)
    );

    void defaultrender_region(
        std::map<std::string,Shader>& shaderManager,
        glm::mat4 mvp,
        glm::vec3 color
    );
    
    static Mesh evalCircle(float radius,uint32_t steps);
    static Mesh evalSphere(float radius,uint32_t steps);
    static Mesh evalQuad();
	static Mesh evalCube();
	static Mesh evalSweepSurf(const Curve& profile, unsigned steps);
	static Mesh evalCylinder(const Curve& profile, const Curve& sweep);
	static Mesh evalModel(const char* filename);

public:
    static Shader defaultShader();
    static Shader defaultShader_N();
    static Shader defaultShader_region(); 
    static Shader defaultShader_phone();
};

class MeshTBO{
    glTBO vertices,indices;
    uint32_t FaceNum;

    public:
    void init(const Mesh& mesh);
    void use();
    void release();
    uint32_t getFaceNum();
};