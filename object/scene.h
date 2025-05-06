#pragma once

#include <set>

#include <glBasic/Camera.h>
#include <object/object.h>
#include <common/IdManager.h>

class Scene{
public:
    std::set<uint32_t> ObjectsSelect;
    uint32_t lastSelectObject=-1;

    std::map<std::uint32_t,shared_ptr<RenderObject>> Objects;
    std::map<std::uint32_t,shared_ptr<Light>> Lights;
    IdManager ObjectsIdManager;
    void addObject(const RenderObject& obj);
    void delObject(uint32_t id);

    void selectObject(uint32_t id);
    void deselectObjcet(uint32_t id);
    void clearSelcectObjects();
    glm::vec3 getObjectsSelectCenter();
    void dragSelectObjects(glm::vec3 offset);

    bool intersect(glm::vec3 RayOrigin,glm::vec3 RayDir,float tmin,float& t,uint32_t& hitid);

    shared_ptr<Light> getLight(){//todo:: multiple light
        if(Lights.empty())throw std::runtime_error("only support one light,but get no light!!!");
        return Lights.begin()->second;
    };

    void render();
    void update();
    void render_select();

    glm::vec3 ambientColor=glm::vec3(0.1f);
    void render_phone();

    static std::function<void(const RenderObject&)> defaultrender(
        Camera* camera,uint32_t* width,uint32_t* height);

    static std::function<void(const RenderObject&)> defaultrender_region(
        Camera* camera,uint32_t* width,uint32_t* height);
};
