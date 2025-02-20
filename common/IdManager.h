#pragma once

#include <vector>

class IdManager{
public:
    uint32_t get(){
        if(freelist.size()==0)return tail++;
        else{
            auto id=freelist.back();
            freelist.pop_back();
            return id;
        } 
    }
    void release(uint32_t id){
        freelist.push_back(id);
    }
private:
    uint32_t tail=0;
    std::vector<uint32_t> freelist;
};