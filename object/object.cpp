#include "object.h"

#include <glad/glad.h>

void RenderObject::render_select(const std::map<std::string,RenderObject>& objects){
    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    for(auto& i:objects){
        i.second.render_margin(i.second);
    }

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

bool RenderObject::intersect(glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t){

    if(clickRegion==nullptr)return false;

    auto model=GetModelMatrix();
    auto modelrev=glm::inverse(model);

    glm::vec3 lightPoint=modelrev*glm::vec4(RayOrigin,1.0f);
    glm::vec3 lightDir=glm::mat3(modelrev)*RayDir; 

    glm::vec3 minPoint=glm::vec3(modelrev*glm::vec4(RayOrigin+tmin*RayDir,1.0f));
    float tminh=glm::length(minPoint-lightPoint);

    float newt=std::numeric_limits<float>::max();

    bool flag=false;
    for(size_t i=0;i<clickRegion->indices.size();i+=3){
        Triangle tri;
        tri[0]=((Surface*)clickRegion)->getPoint(clickRegion->indices[i]).V;
        tri[1]=((Surface*)clickRegion)->getPoint(clickRegion->indices[i+1]).V;
        tri[2]=((Surface*)clickRegion)->getPoint(clickRegion->indices[i+2]).V;
        if(intersect_triangle(tri,lightPoint,lightDir,tminh,newt))flag=true;
    }

    if(flag){
        glm::vec3 hitPoint=model*glm::vec4(lightPoint+lightDir*newt,1.0f);
        newt=glm::length(hitPoint-RayOrigin);
        if(newt>tmin&&newt<t){
            t=newt;
            return true;
        }
        return false; 
    }
    return flag;
}

bool RenderObject::intersect_triangle(Triangle tri,glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t){
    glm::vec3 S=RayOrigin-tri[0];
    glm::vec3 E1=tri[1]-tri[0];
    glm::vec3 E2=tri[2]-tri[0];
    glm::vec3 S1=glm::cross(RayDir,E2);
    glm::vec3 S2=glm::cross(S,E1);

    float S1E1=glm::dot(S1,E1);
    if(fabs(S1E1)<1e-8)return false;
    float newt=glm::dot(S2,E2)/S1E1;
    float b1=glm::dot(S1,S)/S1E1;
    float b2=glm::dot(S2,RayDir)/S1E1;

    if(newt>=tmin&&newt<t&&b1>=0.0f&&b2>=0.0f&&(1-b1-b2)>=0.0f){
        t=newt;
        return true;
    }
    return false;
}