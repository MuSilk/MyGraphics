#pragma once

#include <map>
#include <vector>

#include <glm/glm.hpp>

#include <glBasic/glObject.h>
#include <object/DataObject.h>

struct CurvePoint{
    glm:: vec3 V,T,N,B;
    static const size_t SIZE=12; 
    constexpr static std::initializer_list<uint32_t> PARTITION={3,3,3,3};
};

class Curve:public DataObject,public glObject{
public:
    Curve(){useIndex=false;type=DataType::CURVE;name="Curve";};
    Curve(size_t n):Curve(){vertices.resize(CurvePoint::SIZE*n);}

    CurvePoint& getPoint(size_t index) const;

    size_t size() const{return vertices.size()/CurvePoint::SIZE;};
    CurvePoint& front() const{return getPoint(0);}
    CurvePoint& back() const{return getPoint(size()-1);}
    CurvePoint& operator[](size_t index) const{return getPoint(index);}

    void init();
    void defaultrender(
        glm::mat4 model,glm::mat4 view,glm::mat4 projection,
        glm::vec3 color=glm::vec3(1.0f,1.0f,1.0f)
    );
    void defaultrender_TNB(glm::mat4 model,glm::mat4 view,glm::mat4 projection);

    static Curve evalBezier(const std::vector<glm::vec3>& P,uint32_t steps);
    static Curve evalBspline(const std::vector<glm::vec3>& P,uint32_t steps);
    static Curve evalCircle(float radius,uint32_t steps);

private:
    static Shader defaultShader();
};