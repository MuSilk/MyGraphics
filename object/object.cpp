#include "object.h"

#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glBasic/ShaderManager.h>
#include <object/scene.h>
#include <glBasic/Camera.h>

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
        tri[0]=((Mesh<MeshPoint>*)clickRegion)->getPoint(clickRegion->indices[i]).V;
        tri[1]=((Mesh<MeshPoint>*)clickRegion)->getPoint(clickRegion->indices[i+1]).V;
        tri[2]=((Mesh<MeshPoint>*)clickRegion)->getPoint(clickRegion->indices[i+2]).V;
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

PhoneObject PhoneObject::evalMeshObject(
    glm::vec3 pos,DataObject* datasource,
    Camera& camera,uint32_t& Width,uint32_t& Height
){
    PhoneObject obj;
    obj.position=pos;
    obj.scale=glm::vec3(1.0f,1.0f,1.0f);
    obj.rotation=glm::quat(glm::vec3(0.0f,0.0f,0.0f));
    obj.dataSurface=datasource;
    obj.clickRegion=datasource;

    obj.diffuse=make_shared<SurfaceColor>();
    obj.specular=make_shared<SurfaceColor>();
    obj.shininess=32.0f;

    return obj;
}

void Light::initShadowMap(uint32_t width,uint32_t height){
    shadowMapWidth=width;
    shadowMapHeight=height;

    glGenTextures(1,&shadowCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP,shadowCubeMap);
    for(int i=0;i<6;i++){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_DEPTH_COMPONENT,width,height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    float borderColor[] = { 1.0,1.0,1.0,1.0 };
	glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Light::generateShadowMap(shared_ptr<Scene> scene,float near,float far){
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0,0,shadowMapWidth,shadowMapHeight);
    glBindFramebuffer(GL_FRAMEBUFFER,shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = glm::perspective(glm::radians(90.0f),(float)shadowMapWidth/shadowMapHeight,near,far);
    std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(proj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(proj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(proj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(proj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	shadowTransforms.push_back(proj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	shadowTransforms.push_back(proj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    auto shadowshader=ShaderManager::getInstance().getShader(ShaderType::SHADER_PHONE);//todo 
	
    shadowshader->use();
    for(int i=0;i<6;i++){
       shadowshader->setMat4(("shadowMatrices["+to_string(i)+"]").c_str(), shadowTransforms[i]); 
    }

    shadowshader->setVec3("lightPos", position);
	shadowshader->set1f("far_plane", far);

    // for(auto obj:scene->Objects){
    //     if(obj.second->objectType()==ObjectType::PHONE){
    //         auto real_ptr=dynamic_pointer_cast<PhoneObject>(obj.second);
    //         real_ptr->render_phone(*real_ptr);
    //     }
    // }

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
}

void PhoneObject::defaultrender_phone(
	glm::mat4 model,glm::mat4 view,glm::mat4 projection,
	glm::vec3 viewPos,
	Light* light,
	glm::vec3 ambientColor){

    auto shader=ShaderManager::getInstance().getShader(ShaderType::SHADER_PHONE);
	shader->use();

	shader->setMat4("model",model);
	shader->setMat4("view",view);
	shader->setMat4("projection",projection);

	shader->setVec3("viewPos",viewPos);
    switch (diffuse->surfaceType()){
    case SurfaceType::COLOR:{
        shader->setBool("material.use_diffuse_texture",false);
        shader->setVec4("material.diffuse",glm::vec4((static_pointer_cast<SurfaceColor>(diffuse))->color,1.0f));
        break;
    }
    case SurfaceType::TEXTURE:{
        shader->setBool("material.use_diffuse_texture",true);
        shader->setInt("material.diffuse_texture",0);
        auto diffuse_ptr=static_pointer_cast<SurfaceTexture>(diffuse);
        if(diffuse_ptr->texture)diffuse_ptr->texture->bind(0);
        break;
    }
    default:
        break;
    }
    
    switch (specular->surfaceType()){
    case SurfaceType::COLOR:{
        shader->setBool("material.use_specular_texture",false);
        shader->setVec4("material.specular",glm::vec4(((static_pointer_cast<SurfaceColor>(specular)))->color,1.0f));
        break;
    }
    case SurfaceType::TEXTURE:{
        shader->setBool("material.use_specular_texture",true);
        shader->setInt("material.specular_texture",1);
        auto specular_ptr=static_pointer_cast<SurfaceTexture>(specular);
        if(specular_ptr->texture)specular_ptr->texture->bind(1);
    }
    default:
        break;
    }
    
	shader->set1f("material.shininess",shininess);
	shader->setVec3("light.position",light->position);
	shader->setVec3("light.color",light->color);
	shader->set1f("light.constant",light->constant);
	shader->set1f("light.linear",light->linear);
	shader->set1f("light.quadratic",light->quadratic);
	shader->setVec3("ambientColor",ambientColor);

    ((Mesh<MeshPoint>*)dataSurface)->render(GL_TRIANGLES);
}

void ObjectGroup::addObject(const RenderObject& obj){
    auto id=ObjectsIdManager.get();
    switch (obj.objectType())
    {
    case ObjectType::COMMON:
        Objects[id]=make_shared<RenderObject>(obj);
        if(Objects[id]->name=="")Objects[id]->name=obj.dataSurface->name+std::to_string(id);
        break;
    case ObjectType::PHONE:{
        Objects[id]=make_shared<PhoneObject>(*(PhoneObject*)&obj);
        if(Objects[id]->name=="")Objects[id]->name=obj.dataSurface->name+std::to_string(id);
        break;
    }
    case ObjectType::LIGHT:{
        Objects[id]=Lights[id]=make_shared<Light>(*(Light*)&obj);
        if(Objects[id]->name=="")Objects[id]->name="Light"+std::to_string(id);
        break;
    }
    default:
        break;
    }
    Objects[id]->id=id;
}

void ObjectGroup::delObject(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    // if(Objects[id]->selected)ObjectsSelect.erase(id);
    // if(lastSelectObject==id)lastSelectObject=-1;
    if(Objects[id]->objectType()==ObjectType::LIGHT)Lights.erase(id);
    ObjectsIdManager.release(id);
    Objects.erase(id);
}