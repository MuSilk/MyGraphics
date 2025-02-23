#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>


#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader{
private:
    static void LoadFile(const char* path,std::string& code){
        std::ifstream in(path);
        if(!in.is_open())std::cerr<<"File not find\n";
        in.seekg(0,std::ios::end);
        uint32_t length = (uint32_t)in.tellg();
        in.seekg(0,std::ios::beg);
        code.resize(length+1);
        in.read(code.data(),length+1);
        in.close();
    }
public:
    uint32_t ID;
	
    void compile(const char* vertexSource, const char* geometrySource, const char* fragmentSource);
    void compileFromFile(const char* vertexfile, const char* geometryfile, const char* fragmentfile);

    void release() const {glDeleteProgram(ID);}
    void use() const {glUseProgram(ID);}
    void setBool(const char* name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name), (int)value);
	}
	void setInt(const char* name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name), value);
	}
	void set1f(const char* name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name), value);
	}
	void set2f(const char* name, float valuex, float valuey) {
		glUniform2f(glGetUniformLocation(ID, name), valuex, valuey);
	}
	void set3f(const char* name, float valuex, float valuey, float valuez) {
		glUniform3f(glGetUniformLocation(ID, name), valuex, valuey, valuez);
	}
	void set4f(const char* name, float valuex, float valuey, float valuez, float valuew) {
		glUniform4f(glGetUniformLocation(ID, name), valuex, valuey, valuez, valuew);
	}
	void setVec2(const char* name, glm::vec2 vec) {
		glUniform2f(glGetUniformLocation(ID, name), vec.x, vec.y);
	}
	void setVec3(const char* name, glm::vec3 vec) {
		glUniform3f(glGetUniformLocation(ID, name), vec.x, vec.y, vec.z);
	}
	void setVec4(const char* name, glm::vec4 vec) {
		glUniform4f(glGetUniformLocation(ID, name), vec.x, vec.y, vec.z, vec.w);
	}
	void setMat4(const char* name, glm::mat4 mat) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, &mat[0][0]);
	}

	static Shader Shader_2drgb();
	static Shader Shader_3drgb();
	static Shader Shader_3d_single_color();
	static Shader Shader_v3c3mvp();
	static Shader Shader_curve();
	static Shader Shader_surface();
	static Shader Shader_surface_phone();
	static Shader Shader_ray_trace();
	static Shader Shader_screen();
};

#include <map>
typedef std::map<std::string,Shader> ShaderManager;