#include "scene.h"

void Scene::addObject(const RenderObject& obj){
    auto id=ObjectsIdManager.get();
    switch (obj.objectType())
    {
    case ObjectType::COMMON:
        Objects[id]=new RenderObject(obj);
        if(Objects[id]->name=="")Objects[id]->name=obj.dataSurface->name+std::to_string(id);
        break;
    case ObjectType::PHONE:{
        Objects[id]=new PhoneObject(*((PhoneObject*)&obj));
        if(Objects[id]->name=="")Objects[id]->name=obj.dataSurface->name+std::to_string(id);
        break;
    }
    case ObjectType::LIGHT:
        Objects[id]=new Light(*((Light*)&obj));
        if(Objects[id]->name=="")Objects[id]->name="Light"+std::to_string(id);
        break;
    default:
        break;
    }
    Objects[id]->id=id;
    Objects[id]->curScene=this;
}
void Scene::delObject(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    if(Objects[id]->selected)ObjectsSelect.erase(id);
    if(lastSelectObject==id)lastSelectObject=-1;
    ObjectsIdManager.release(id);
    delete Objects[id];
    Objects.erase(id);
}

void Scene::selectObject(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    lastSelectObject=id;
    Objects[id]->selected=true;
    ObjectsSelect.insert(id);
}

void Scene::deselectObjcet(uint32_t id){
    if(Objects.find(id)==Objects.end())return;
    Objects[id]->selected=false;
    ObjectsSelect.erase(id);
}

void Scene::clearSelcectObjects(){
    for(auto& id:ObjectsSelect){
        Objects[id]->selected=false;
    }
    ObjectsSelect.clear();
}

glm::vec3 Scene::getObjectsSelectCenter(){
    if(ObjectsSelect.empty())return glm::vec3(0.0f);
    glm::vec3 center(0.0f);
    for(auto& id:ObjectsSelect){
        center+=Objects[id]->position;
    }
    return center/float(ObjectsSelect.size());
}

bool Scene::intersect(glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t,uint32_t& hitid){
    bool flag=false;
    for(auto& [id,obj]:Objects){
        if(obj->intersect(RayOrigin,RayDir,tmin,t)){
            hitid=id;
            flag=true;
        }
    }
    return flag;
}

void Scene::dragSelectObjects(glm::vec3 offset){
    glm::mat4 matrix=glm::translate(glm::mat4(1.0f),offset);
    for(auto& i:ObjectsSelect){
        Objects[i]->position=glm::vec3(matrix*glm::vec4(Objects[i]->position,1.0f));   
    }
}

void Scene::render(){
    for(auto& [id,obj]:Objects){
        obj->render(*obj);
    }
}

void Scene::render_phone(){
    for(auto [id,obj]:Objects){
        obj->render_phone(*obj);
    }
}
void Scene::render_select(){
    glEnable(GL_STENCIL_TEST);

    glStencilFunc(GL_NOTEQUAL,1,0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    for(auto& id:ObjectsSelect){
        Objects[id]->render_margin(*Objects[id]);
    }

    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

void Scene::update(){
    for(auto& [id,obj]:Objects){
        obj->update(*obj);
    }
}

std::function<void(const RenderObject&)> Scene::defaultrender(
    Camera* camera,uint32_t* width,uint32_t* height){

    return [camera,width,height](const RenderObject& thisobj){
        glm::mat4 model=thisobj.GetModelMatrix();
        glm::mat4 view=camera->GetViewMatrix();
        glm::mat4 proj=camera->GetProjectionMatrix(*width,*height);
        glm::vec3 lightdir=glm::normalize(-camera->Front);
        
        if(thisobj.selected){
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP,GL_REPLACE,GL_REPLACE);
            glStencilFunc(GL_ALWAYS,1,0xFF);

            switch (thisobj.dataSurface->type){
            case DataType::MESH:
                ((Mesh*)thisobj.dataSurface)->defaultrender(model,view,proj,camera->Position,lightdir);
                break;
            case DataType::CURVE:
                ((Curve*)thisobj.dataSurface)->defaultrender(model,view,proj);
                break;
            default:
                break;
            }
            
            glDisable(GL_STENCIL_TEST);
        }
        else {
            switch (thisobj.dataSurface->type){
            case DataType::MESH:
                ((Mesh*)thisobj.dataSurface)->defaultrender(model,view,proj,camera->Position,lightdir);
                break;
            case DataType::CURVE:
                ((Curve*)thisobj.dataSurface)->defaultrender(model,view,proj);
                break;
            default:
                break;
            }
        }
    };

}

std::function<void(const RenderObject&)> Scene::defaultrender_region(
    Camera* camera,uint32_t* width,uint32_t* height){
    return [camera,width,height](const RenderObject& thisobj){
        auto scale=thisobj.scale;
        scale*=1.03f;
        glm::mat4 model=thisobj.GetTranslateMatrix()*thisobj.GetRotateMatrix()*glm::scale(glm::mat4(1.0f),scale);
        glm::mat4 view=camera->GetViewMatrix();
        glm::mat4 proj=camera->GetProjectionMatrix(*width,*height);

        switch (thisobj.dataSurface->type){
            case DataType::MESH:
                ((Mesh*)thisobj.dataSurface)->defaultrender_region(proj*view*model,glm::vec3(1.0f,1.0f,0.0f));
                break;
            case DataType::CURVE:
                ((Curve*)thisobj.dataSurface)->defaultrender(model,view,proj,glm::vec3(1.0f,1.0f,0.0f));
                break;
            default:
                break;
            }
    };
}
