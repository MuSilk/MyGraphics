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
        tri[0]=((Mesh*)clickRegion)->getPoint(clickRegion->indices[i]).V;
        tri[1]=((Mesh*)clickRegion)->getPoint(clickRegion->indices[i+1]).V;
        tri[2]=((Mesh*)clickRegion)->getPoint(clickRegion->indices[i+2]).V;
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

#include <object/scene.h>
#include <glBasic/Camera.h>

RenderObject RenderObject::evalSurface(
    glm::vec3 pos,DataObject* datasource,
    ShaderManager& shaderManager,Camera& camera,
    uint32_t& Width,uint32_t& Height
){
    RenderObject obj;
    obj.position=pos;
    obj.scale=glm::vec3(1.0f,1.0f,1.0f);
    obj.rotation=glm::quat(glm::vec3(0.0f,0.0f,0.0f));
    obj.dataSurface=datasource;
    obj.clickRegion=datasource;

    obj.render=Scene::defaultrender(shaderManager,&camera,&Width,&Height);
    obj.render_margin=Scene::defaultrender_region(shaderManager,&camera,&Width,&Height);

    return obj;
}

PhoneObject PhoneObject::evalMeshObject(
    glm::vec3 pos,DataObject* datasource,
    ShaderManager& shaderManager,Camera& camera,
    uint32_t& Width,uint32_t& Height
){
    PhoneObject obj;
    obj.position=pos;
    obj.scale=glm::vec3(1.0f,1.0f,1.0f);
    obj.rotation=glm::quat(glm::vec3(0.0f,0.0f,0.0f));
    obj.dataSurface=datasource;
    obj.clickRegion=datasource;

    obj.render=Scene::defaultrender(shaderManager,&camera,&Width,&Height);
    obj.render_margin=Scene::defaultrender_region(shaderManager,&camera,&Width,&Height);

    obj.render_phone=[&](const RenderObject& thisobj){
        glm::mat4 model=thisobj.GetModelMatrix();
        glm::mat4 view=camera.GetViewMatrix();
        glm::mat4 proj=camera.GetProjectionMatrix(Width,Height);
        glm::vec3 lightdir=glm::normalize(-camera.Front);

        Light* light=thisobj.curScene->getLight();
        
        ((PhoneObject*)&thisobj)->defaultrender_phone(
            shaderManager,model,view,proj,
            camera.Position,
            light,thisobj.curScene->ambientColor
        );
    };

    obj.diffuse=new SurfaceColor();
    obj.specular=new SurfaceColor();
    obj.shininess=32.0f;

    return obj;
}

#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

void PhoneObject::defaultrender_phone(
	std::map<std::string,Shader>& shaderManager,
	glm::mat4 model,glm::mat4 view,glm::mat4 projection,
	glm::vec3 viewPos,
	Light* light,
	glm::vec3 ambientColor){

	if(shaderManager.find("PHONE_DEFAULT_SHADER")==shaderManager.end()){
		shaderManager["PHONE_DEFAULT_SHADER"] = Mesh::defaultShader_phone();
	}
	Shader shader=shaderManager["PHONE_DEFAULT_SHADER"];
	shader.use();

	shader.setMat4("model",model);
	shader.setMat4("view",view);
	shader.setMat4("projection",projection);

	shader.setVec3("viewPos",viewPos);
	shader.setVec3("material.diffuse",((SurfaceColor*)diffuse)->color);
	shader.setVec3("material.specular",((SurfaceColor*)specular)->color);
	shader.set1f("material.shininess",shininess);
	shader.setVec3("light.position",light->position);
	shader.setVec3("light.color",light->color);
	shader.set1f("light.constant",light->constant);
	shader.set1f("light.linear",light->linear);
	shader.set1f("light.quadratic",light->quadratic);
	shader.setVec3("ambientColor",ambientColor);

    ((Mesh*)dataSurface)->render(GL_TRIANGLES);
}