#include <memory>

#include <glm/glm.hpp>
#include <tinyxml2.h>
using namespace tinyxml2;

class ProjectSerializer {
public:
    template<typename T>
    static XMLElement* serialize(XMLDocument& doc,std::shared_ptr<T> obj);

    template<typename T>
    static XMLElement* serialize(XMLDocument& doc,const char* name,std::shared_ptr<T> obj);

    template<typename T>
    static std::shared_ptr<T> deserialize(XMLElement* elem);

    template<typename T>
    static XMLElement* serializeValue(XMLDocument& doc,const char* name,T value){
        auto elem=doc.NewElement(name);
        elem->SetAttribute("Value",value);
        return elem;
    }
    static XMLElement* serializeColor(XMLDocument& doc,const char* name,glm::vec3 color);
    static XMLElement* serializeVec(XMLDocument& doc,const char* name,glm::vec3 vec);
    static XMLElement* serializeVec(XMLDocument& doc,const char* name,glm::vec4 vec);
    static XMLElement* serializeQuat(XMLDocument& doc,const char* name,glm::quat vec);

    static glm::vec3 deserializeColor(XMLElement* elem);
    static glm::vec3 deserializeVec3(XMLElement* elem);
    static glm::vec4 deserializeVec4(XMLElement* elem);
    static glm::quat deserializeQuat(XMLElement* elem);
private:
    template<typename T>
    static XMLElement* serializeBase(XMLDocument& doc,std::shared_ptr<T> obj);
};