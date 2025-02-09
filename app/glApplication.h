#pragma once

#include "Application.h"

#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glBasic/Shader.h>
#include <glBasic/Camera.h>

class glApplication:public Application{
private:
    const uint32_t WIDTH=1600;
    const uint32_t HEIGHT=1200;
    uint32_t Width,Height;
    GLFWwindow* window;

    std::map<std::string,Shader> Shaders;

    Camera MainCamera;

    bool MouseEnabled=true;
    bool firstMouse=true;

    ImGuiIO* io;

    void initWindow() override;
    void initGraphics() override;
    void mainLoop() override;
    void cleanUp() override;

    void createShaders();
	void createDataBuffer();
	void initSettings();

    void GuiRender();
    void EngineRender();
    void EngineUpdate(double dt);

    double time,now,deltaTime;
    static void framebufferResizeCallback(GLFWwindow* window,int width,int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void cursorPosCallback(GLFWwindow* window, double xposIn, double yposIn);
};