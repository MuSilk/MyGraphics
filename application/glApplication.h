#pragma once

#include "Application.h"

#include <map>
#include <set>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <common/IdManager.h>

#include <glBasic/Shader.h>
#include <glBasic/Camera.h>

#include <object/DataObject.h>
#include <object/object.h>
#include <object/scene.h>

class glApplication:public Application{
private:
    const uint32_t WIDTH=1600;
    const uint32_t HEIGHT=1200;
    uint32_t Width=WIDTH,Height=HEIGHT;
    GLFWwindow* window;

    struct sourcedTexture{
        std::shared_ptr<Texture2D> texture;
        std::string source;
    };

    std::map<std::string,sourcedTexture> Textures;
    std::map<std::string,std::shared_ptr<DataObject>> DataObjects;

    Scene MainScene;

    Camera MainCamera;

    bool MouseEnabled=true;
    bool firstMouse=true;
    glm::vec2 mousePos;

    ImGuiIO* io;
    glm::vec2 mouseRightClickMenuPos;
    glm::vec2 mouseLeftClickPos;
    glm::vec2 ViewBoxPos; 
    glm::vec2 ViewBoxSize;

    void initWindow() override;
    void initGraphics() override;
    void mainLoop() override;
    void cleanUp() override;

    void createShaders();
	void createDataBuffer();
	void initSettings();

    Scene::RenderOption renderOption;

    void GuiRender();
    void EngineRender();
    void EngineUpdate(double dt);

    void dragSelectObjects(glm::vec2 mouseMovement);

    void saveProject(const char* path);
    void loadProject(const char* path);

    bool MouseStatus[8];

    double time,now,deltaTime;
    static void framebufferResizeCallback(GLFWwindow* window,int width,int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void cursorPosCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mode);
};