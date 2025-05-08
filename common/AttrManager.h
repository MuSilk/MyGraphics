#include <glm/glm.hpp>
#include <tinyxml2.h>
#include <string>
using namespace tinyxml2;

class AttrManager{
public:
    // AttrManager(){
    //     root = doc.NewElement("Attrs");
    // }
    
    // template<typename T>
    // void setAttr(const char* name,T value){
    //     auto node = doc.NewElement(name);
    //     node->SetValue(std::to_string(value).c_str());
    //     root->InsertEndChild(node);
    // }

    // XMLNode* getAttr(XMLDocument* dst) const { 
    //     return root->DeepClone(dst); 
    // }

private:
    XMLDocument doc;
    // XMLElement* root;
};