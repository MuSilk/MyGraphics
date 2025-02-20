#include "glDynamicObject.h"

void glDynamicObject::init(size_t size,std::initializer_list<uint32_t> partition){
    sum=0;
    for(auto it:partition)sum+=it;
    _size=size;
    useEBO=false;

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,size*sum*sizeof(float),NULL,GL_DYNAMIC_DRAW);

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

void glDynamicObject::init(size_t size,std::initializer_list<uint32_t> partition,std::vector<uint32_t> indices){
    sum = 0;
    for (const auto& it : partition)sum += it;
    _size = indices.size();
    useEBO = true;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sum * sizeof(float), NULL, GL_DYNAMIC_DRAW);

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

void glDynamicObject::update(const float* const vertices){
    glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _size * sum * sizeof(float), vertices);
}

void glDynamicObject::render(GLenum mode){
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (useEBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(mode, _size, GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(mode, 0, _size);
    }
}

void glDynamicObject::release(){
    if(useEBO)glDeleteBuffers(1,&EBO);
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
}