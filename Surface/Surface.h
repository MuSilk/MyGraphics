#pragma once

#include <glBasic/glObject.h>
#include "Curve.h"

struct SurfacePoint{
    glm::vec3 V,N;
};

class Surface:public glObject{
public:
    std::vector<SurfacePoint> vertices;
    std::vector<uint32_t> indices;

    void init();
    void defaultrender(
        std::map<std::string,Shader>& shaderManager,
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 viewPos,
        glm::vec3 lightDir=glm::normalize(glm::vec3(-1.0f,1.0f,0.0f)),
        glm::vec3 materialDiffuse=glm::vec3(0.4f),
        glm::vec3 materialSpecular=glm::vec3(0.4f),
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
        glm::vec3 materialSpecular=glm::vec3(0.4f),
        float materialShininess=20.0f,
        glm::vec3 lightColor=glm::vec3(1.0f),
        glm::vec3 ambientColor=glm::vec3(0.5f)
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
};

