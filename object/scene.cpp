#include "scene.h"

void Scene::addObject(RenderObject& obj){
    obj.id=ObjectsIdManager.get();
    if(obj.name=="")obj.name="Object"+std::to_string(obj.id);
    Objects[obj.id]=obj;
}
void Scene::delObject(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    if(Objects[id].selected)ObjectsSelect.erase(id);
    if(lastSelectObject==id)lastSelectObject=-1;
    ObjectsIdManager.release(id);
    Objects.erase(id);
}

void Scene::selectObject(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    lastSelectObject=id;
    Objects[id].selected=true;
    ObjectsSelect.insert(id);
}

void Scene::deselectObjcet(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    Objects[id].selected=false;
    ObjectsSelect.erase(id);
}

void Scene::clearSelcectObjects(){
    for(auto& id:ObjectsSelect){
        Objects[id].selected=false;
    }
    ObjectsSelect.clear();
}

glm::vec3 Scene::getObjectsSelectCenter(){
    if(ObjectsSelect.empty())return glm::vec3(0.0f);
    glm::vec3 center(0.0f);
    for(auto& id:ObjectsSelect){
        center+=Objects[id].position;
    }
    return center/float(ObjectsSelect.size());
}
bool Scene::intersect(glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t,uint32_t& hitid){
    bool flag=false;
    for(auto& [id,obj]:Objects){
        if(obj.intersect(RayOrigin,RayDir,tmin,t)){
            hitid=id;
            flag=true;
        }
    }
    return flag;
}
void Scene::render(){
    for(auto& i:Objects){
        i.second.render(i.second);
    }
}

void Scene::render_select(){
    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    for(auto& id:ObjectsSelect){
        Objects[id].render_margin(Objects[id]);
    }

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

std::function<void(const RenderObject&)> Scene::defaultrender(
    std::map<std::string,Shader>& shaderManager,
    Camera* camera,uint32_t* width,uint32_t* height){
    return [&shaderManager,camera,width,height](const RenderObject& thisobj){
        glm::mat4 model=thisobj.GetModelMatrix();
        glm::mat4 view=camera->GetViewMatrix();
        glm::mat4 proj=camera->GetProjectionMatrix(*width,*height);
        glm::vec3 lightdir=glm::normalize(-camera->Front);
        
        if(thisobj.selected){
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP,GL_REPLACE,GL_REPLACE);
            glStencilFunc(GL_ALWAYS,1,0xFF);
            ((Surface*)thisobj.surface)->defaultrender(shaderManager,model,view,proj,camera->Position,lightdir);
            glDisable(GL_STENCIL_TEST);
        }
        else ((Surface*)thisobj.surface)->defaultrender(shaderManager,model,view,proj,camera->Position,lightdir);
    };
}

std::function<void(const RenderObject&)> Scene::defaultrender_region(
    std::map<std::string,Shader>& shaderManager,
    Camera* camera,uint32_t* width,uint32_t* height){
    return [&shaderManager,camera,width,height](const RenderObject& thisobj){
        auto scale=thisobj.scale;
        scale*=1.03f;
        glm::mat4 model=thisobj.GetTranslateMatrix()*thisobj.GetRotateMatrix()*glm::scale(glm::mat4(1.0f),scale);
        glm::mat4 view=camera->GetViewMatrix();
        glm::mat4 proj=camera->GetProjectionMatrix(*width,*height);
        ((Surface*)thisobj.surface)->defaultrender_region(shaderManager,proj*view*model,glm::vec3(1.0f,1.0f,0.0f));
    };
}
