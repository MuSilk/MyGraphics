#pragma once

#include <glBasic/glObject.h>
#include <glBasic/glTBO.h>
#include <object/DataObject.h>
#include "Curve.h"

struct SurfacePoint{
    glm::vec3 V,N;
    static const size_t SIZE=6; 
    constexpr static std::initializer_list<uint32_t> PARTITION={3,3};
};

class Surface:public glObject,public DataObject{
public:
    Surface(){useIndex=true;}
    void init();

    SurfacePoint& getPoint(uint32_t index) const;
    size_t size() const{return vertices.size()/SurfacePoint::SIZE;};
    SurfacePoint& front() const{return getPoint(0);}
    SurfacePoint& back() const{return getPoint(size()-1);}
    SurfacePoint& operator[](uint32_t index) const{return getPoint(index);}

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
    
    static Surface evalSphere(float radius,uint32_t steps);
    static Surface evalQuad();
	static Surface evalCube();
	static Surface evalSweepSurf(const Curve& profile, unsigned steps);
	static Surface evalCylinder(const Curve& profile, const Curve& sweep);
	static Surface evalModel(const char* filename);

private:
    static Shader defaultShader();
    static Shader defaultShader_N();
    static Shader defaultShader_region(); 
};

class SurfaceTBO{
    glTBO vertices,indices;
    uint32_t FaceNum;

    public:
    void init(const Surface& surface);
    void use();
    void release();
    uint32_t getFaceNum();
};