#include "ScreenVT.h"

void ScreenVT::init(){
    glObject::init(reinterpret_cast<float*>(vertices.data()),vertices.size(),{2,2},indices);
}

void ScreenVT::render(){
    glObject::render(GL_TRIANGLES);
}

ScreenVT ScreenVT::evalQuad(){
    ScreenVT R;
    R.vertices.insert(R.vertices.end(), {
		{glm::vec2(-1,-1),glm::vec2(0,0)},
		{glm::vec2(+1,-1),glm::vec2(1,0)},
		{glm::vec2(+1,+1),glm::vec2(1,1)},
		{glm::vec2(-1,+1),glm::vec2(0,1)},
	});

	R.indices.insert(R.indices.end(), {
		0,1,2,
		0,2,3
	});

	return R;
}

void ScreenVTFBO::init(int width,int height){
    glGenFramebuffers(1,&FBO);
    glBindFramebuffer(GL_FRAMEBUFFER,FBO);

    glGenTextures(1,&TEX);
    glBindTexture(GL_TEXTURE_2D,TEX);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "frame create failed\n";
    }
}

void ScreenVTFBO::use(){
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void ScreenVTFBO::unuse(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ScreenVTFBO::useAsTexture(){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TEX);
}

void ScreenVTFBO::release(){
    unuse();
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1,&TEX);    
}