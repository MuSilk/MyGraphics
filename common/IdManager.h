#pragma once

#include <vector>

class IdManager{
public:
    uint32_t get(){return tail++;}
    void release(uint32_t id){return;}
    void setfirstUnusedId(uint32_t id){tail=id;}

    uint32_t tail=0;    
};