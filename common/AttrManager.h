#include <glm/glm.hpp>
#include <tinyxml2.h>
#include <string>
using namespace tinyxml2;

class AttrManager{
public:
    AttrManager(){
        root = doc.NewElement("Attrs");
    }
    
    template<typename T>
    void setAttr(const char* name,T value){
        auto node = doc.NewElement(name);
        node->SetAttribute("value",std::to_string(value).c_str());
        root->InsertEndChild(node);
    }

    template<>
    void setAttr(const char* name,std::string value){
        auto node = doc.NewElement(name);
        node->SetAttribute("value",value.c_str());
        root->InsertEndChild(node);
    }

    template<>
    void setAttr(const char* name,const char* value){
        auto node = doc.NewElement(name);
        node->SetAttribute("value",value);
        root->InsertEndChild(node);
    }



    XMLNode* getAttrs(XMLDocument* dst) const { 
        return root->DeepClone(dst); 
    }

private:
    XMLDocument doc;
    XMLElement* root;
};