#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include <glBasic/glObject.h>
#include <object/DataObject.h>

struct CurvePoint{
    glm:: vec3 V,T,N,B;
};

class Curve:public DataObject,public glObject{
public:
    Curve(){useIndex=false;};
    Curve(size_t n){vertices.resize(12*n);}

    CurvePoint& getPoint(uint32_t index) const;

    size_t size() const{return vertices.size()/12;};
    CurvePoint& front() const{return getPoint(0);}
    CurvePoint& back() const{return getPoint(size()-1);}
    CurvePoint& operator[](uint32_t index) const{return getPoint(index);}

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