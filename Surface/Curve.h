#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include <glBasic/glObject.h>

struct CurvePoint{
    glm:: vec3 V,T,N,B;
};

class Curve:public std::vector<CurvePoint>,public glObject{
public:
    Curve(){};
    Curve(size_t n):std::vector<CurvePoint>(n){}

    void init();
    void defaultrender(std::map<std::string,Shader>& shaderManager,glm::mat4 model,glm::mat4 view,glm::mat4 projection);
    void defaultrender_TNB(std::map<std::string,Shader>& shaderManager,glm::mat4 model,glm::mat4 view,glm::mat4 projection);

    static Curve evalBezier(const std::vector<glm::vec3>& P,uint32_t steps);
    static Curve evalBspline(const std::vector<glm::vec3>& P,uint32_t steps);
    static Curve evalCircle(float radius,uint32_t steps);

private:
    static Shader defaultShader();
    static Shader defaultShader_TNB();
};