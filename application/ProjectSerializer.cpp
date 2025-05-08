#include "ProjectSerializer.h"

#include "object/object.h"

XMLElement* ProjectSerializer::serializeVec(XMLDocument& doc,const char* name,glm::vec3 vec){
    auto elem=doc.NewElement(name);
    elem->SetAttribute("X",vec.x);
    elem->SetAttribute("Y",vec.y);
    elem->SetAttribute("Z",vec.z);
    return elem;
}

glm::vec3 ProjectSerializer::deserializeVec3(XMLElement* elem){
    glm::vec3 vec={
        elem->FloatAttribute("X",1.0f),
        elem->FloatAttribute("Y",1.0f),
        elem->FloatAttribute("Z",1.0f)
    };
    return vec;
}

XMLElement* ProjectSerializer::serializeVec(XMLDocument& doc,const char* name,glm::vec4 vec){
    auto elem=doc.NewElement(name);
    elem->SetAttribute("W",vec.w);
    elem->SetAttribute("X",vec.x);
    elem->SetAttribute("Y",vec.y);
    elem->SetAttribute("Z",vec.z);
    return elem;
}

glm::vec4 ProjectSerializer::deserializeVec4(XMLElement* elem){
    glm::vec4 vec={
        elem->FloatAttribute("W"),
        elem->FloatAttribute("X"),
        elem->FloatAttribute("Y"),
        elem->FloatAttribute("Z")
    };
    return vec;
}

XMLElement* ProjectSerializer::serializeQuat(XMLDocument& doc,const char* name,glm::quat vec){
    auto elem=doc.NewElement(name);
    elem->SetAttribute("W",vec.w);
    elem->SetAttribute("X",vec.x);
    elem->SetAttribute("Y",vec.y);
    elem->SetAttribute("Z",vec.z);
    return elem;
}

glm::quat ProjectSerializer::deserializeQuat(XMLElement* elem){
    glm::quat vec={
        elem->FloatAttribute("W"),
        elem->FloatAttribute("X"),
        elem->FloatAttribute("Y"),
        elem->FloatAttribute("Z")
    };
    return vec;
}

XMLElement* ProjectSerializer::serializeColor(XMLDocument& doc,const char* name,glm::vec3 color){
    auto elem=doc.NewElement(name);
    elem->SetAttribute("R",color.r);
    elem->SetAttribute("G",color.g);
    elem->SetAttribute("B",color.b);
    return elem;
}

glm::vec3 ProjectSerializer::deserializeColor(XMLElement* elem){
    glm::vec3 color;
    color.r=elem->FloatAttribute("R",1.0f);
    color.g=elem->FloatAttribute("G",1.0f);
    color.b=elem->FloatAttribute("B",1.0f);
    return color;
}

template<>
XMLElement* ProjectSerializer::serialize(XMLDocument& doc,shared_ptr<SurfaceColor> obj){
    return serializeColor(doc,"Color",obj->color);
}

template<>
std::shared_ptr<SurfaceColor> ProjectSerializer::deserialize(XMLElement* elem){
    std::shared_ptr<SurfaceColor> obj=std::make_shared<SurfaceColor>();
    obj->color=deserializeColor(elem->FirstChildElement("Color"));
    return obj;
}

template<>
XMLElement* ProjectSerializer::serialize(XMLDocument& doc,shared_ptr<SurfaceTexture> obj){
    return serializeValue(doc,"Name",obj->textureName.c_str());
}

template<>
std::shared_ptr<SurfaceTexture> ProjectSerializer::deserialize(XMLElement* elem){
    std::shared_ptr<SurfaceTexture> obj=std::make_shared<SurfaceTexture>();
    obj->textureName=elem->FirstChildElement("Name")->Attribute("Value");
    return obj;
}

template<>
XMLElement* ProjectSerializer::serialize(XMLDocument& doc,const char* name,shared_ptr<Surface> obj){
    auto elem=doc.NewElement(name);
    elem->SetAttribute("Type",to_string(obj->surfaceType()).c_str());
    if(obj->surfaceType()==SurfaceType::COLOR){
        elem->InsertEndChild(serialize(doc,dynamic_pointer_cast<SurfaceColor>(obj)));
    }
    else if(obj->surfaceType()==SurfaceType::TEXTURE){
        elem->InsertEndChild(serialize(doc,dynamic_pointer_cast<SurfaceTexture>(obj)));
    }
    return elem;
}

template<>
std::shared_ptr<Surface> ProjectSerializer::deserialize(XMLElement* elem){
    auto type=elem->Attribute("Type");
    if(type==to_string(SurfaceType::COLOR)){
        return deserialize<SurfaceColor>(elem);
    }
    else if(type==to_string(SurfaceType::TEXTURE)){
        return deserialize<SurfaceTexture>(elem);
    }
    throw std::runtime_error("Unknow Surface Type");
}

template<>
XMLElement* ProjectSerializer::serializeBase(XMLDocument& doc,shared_ptr<RenderObject> obj){

    auto object_elem=doc.NewElement("Object");

    object_elem->SetAttribute("Id",obj->id);
    object_elem->SetAttribute("Name",obj->name.c_str());
    object_elem->SetAttribute("ObjectType",to_string(obj->objectType()).c_str());

    auto transform_elem=doc.NewElement("Transform");
    object_elem->InsertEndChild(transform_elem);
    transform_elem->InsertEndChild(serializeVec(doc,"Position",obj->position));
    transform_elem->InsertEndChild(serializeQuat(doc,"Rotation",obj->rotation));
    transform_elem->InsertEndChild(serializeVec(doc,"Scale",obj->scale));

    auto dataSurface_elem=doc.NewElement("DataSurface");
    object_elem->InsertEndChild(dataSurface_elem);
    dataSurface_elem->SetAttribute("Name",obj->dataSurface->name.c_str());

    auto clickRegion_elem=doc.NewElement("ClickRegion");
    object_elem->InsertEndChild(clickRegion_elem);
    clickRegion_elem->SetAttribute("Name",obj->clickRegion->name.c_str());

    return object_elem;
}

template<>
XMLElement* ProjectSerializer::serialize(XMLDocument& doc,shared_ptr<Light> obj){
    auto elem=serializeBase(doc,static_pointer_cast<RenderObject>(obj));
    auto lightElem=doc.NewElement("Light");
    elem->InsertEndChild(lightElem);

    lightElem->InsertEndChild(serializeColor(doc,"Color",obj->color));
    lightElem->InsertEndChild(serializeValue(doc,"Constant",obj->constant));
    lightElem->InsertEndChild(serializeValue(doc,"Linear",obj->linear));
    lightElem->InsertEndChild(serializeValue(doc,"Quadratic",obj->quadratic));

    return elem;
}

template<>
XMLElement* ProjectSerializer::serialize(XMLDocument& doc,shared_ptr<PhoneObject> obj){
    auto objectElem=serializeBase(doc,static_pointer_cast<RenderObject>(obj));
    auto phoneElem=doc.NewElement("Phone");
    objectElem->InsertEndChild(phoneElem);

    phoneElem->InsertEndChild(ProjectSerializer::serialize(doc,"Diffuse",obj->diffuse));
    phoneElem->InsertEndChild(ProjectSerializer::serialize(doc,"Specular",obj->specular));
    phoneElem->InsertEndChild(ProjectSerializer::serializeValue(doc,"Shininess",obj->shininess));

    return objectElem;
}

template<>
XMLElement* ProjectSerializer::serialize(XMLDocument& doc,shared_ptr<RenderObject> obj){
    switch (obj->objectType()){
    case ObjectType::LIGHT:
        return serialize(doc,dynamic_pointer_cast<Light>(obj));
    case ObjectType::PHONE:
        return serialize(doc,dynamic_pointer_cast<PhoneObject>(obj));
    default:
        throw std::runtime_error("Unsupported object type");
        break;
    }
    return nullptr;
}