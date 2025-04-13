#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

class Texture2D{
public:
    GLuint ID;

    GLuint Width,Height;

    GLuint InternalFormat=GL_RGBA;
    GLuint ImageFormat=GL_RGBA;

    GLuint Wrap_S = GL_REPEAT;
	GLuint Wrap_T = GL_REPEAT;
	GLuint Filter_Min = GL_LINEAR;
	GLuint Filter_Max = GL_LINEAR;

    void Generate(const char* file);

	void Generate(int width, int height,const unsigned char* data) {
		glGenTextures(1, &ID);

		Width = width;
		Height = height;

		glBindTexture(GL_TEXTURE_2D, ID);
		glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, ImageFormat, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Filter_Min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Filter_Max);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Bind() const {
		glBindTexture(GL_TEXTURE_2D, ID);
	}

	void Delete() {
		glDeleteTextures(1, &ID);
	}
};