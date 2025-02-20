#include "glTBO.h"

void glTBO::init(void* data,size_t size,GLenum type){
    this->type = type;
	glGenTextures(1, &TEX);
	glGenBuffers(1, &TBO);
	glBindBuffer(GL_TEXTURE_BUFFER, TBO);
	glBufferData(GL_TEXTURE_BUFFER, size, data, GL_STATIC_DRAW);
}
void glTBO::bind(int id){
    glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_BUFFER, TEX);
	glTexBuffer(GL_TEXTURE_BUFFER, type, TBO);
}
void glTBO::release(){
    glDeleteBuffers(1, &TBO);
	glDeleteTextures(1, &TEX);
}