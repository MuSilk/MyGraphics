#include "glObject.h"

void glObject::init(const float* const vertices,size_t size,std::initializer_list<uint32_t> partition){
    size_t sum=0;
    for(const auto& it:partition)sum+=it;
    _size=size;
    useEBO=false;

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,size*sum*sizeof(float),vertices,GL_STATIC_DRAW);

    size_t i=0;
    size_t pos=0;
    for(const auto& it:partition){
        glVertexAttribPointer(i,it,GL_FLOAT,GL_FALSE,sum*sizeof(float),(void*)pos);
        glEnableVertexAttribArray(i);
        i++;
        pos+=it*sizeof(float);
    }

    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void glObject::init(const float* const vertices, size_t size, std::initializer_list<uint32_t> partition, const std::vector<uint32_t>& indices) {
    size_t sum = 0;
    for (const auto& it : partition)sum += it;
    _size = indices.size();
    useEBO = true;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sum * sizeof(float), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

    size_t i = 0;
    size_t pos = 0;
    for (const auto& it : partition) {
        glVertexAttribPointer(i, it, GL_FLOAT, GL_FALSE, sum * sizeof(float), (void*)pos);
        glEnableVertexAttribArray(i);
        i++;
        pos += it * sizeof(float);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void glObject::render(GLenum mode){
    glBindVertexArray(VAO);
    if (useEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(mode, _size, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(mode, 0, _size);
    }
}

void glObject::release(){
    if(useEBO)glDeleteBuffers(1,&EBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
}