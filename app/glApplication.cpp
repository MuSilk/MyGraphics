#include "glApplication.h"

#include <stdexcept>
#include <vector>

#define FRAME_PRE_SECOND 120

void glApplication::initWindow(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(WIDTH,HEIGHT,"mainWindow",nullptr,nullptr);
    if(!window) throw std::runtime_error("failed to create window");
    glfwSetWindowUserPointer(window,this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetKeyCallback(window,keyCallback);
    glfwSetCursorPosCallback(window,cursorPosCallback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("failed to initalize GLAD");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io=&ImGui::GetIO();(void)io;

    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void glApplication::initGraphics(){
    createShaders();
    createDataBuffer();
    initSettings();
}

void glApplication::initSettings(){
    glViewport(0,0,WIDTH,HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT,GL_FILL);

    glPointSize(5);

    MainCamera.Position=glm::vec3(-1.0f,1.0f,10.0f);
}

#include <Surface/Curve.h>
#include <Surface/Surface.h>
Surface surface;
Curve curve;

void glApplication::createShaders(){

}

void glApplication::createDataBuffer(){
    surface=Surface::evalSphere(1.0f,50);
    // surface=Surface::evalModel("E:\\program\\c++\\MyEngine\\bunny_200.obj");
    surface.init();
}

void glApplication::mainLoop(){
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        now=glfwGetTime();
        deltaTime=now-time;
        time=now;

        static double dt=0;
        dt+=deltaTime;
        if(dt>1.0/FRAME_PRE_SECOND){
            EngineUpdate(1.0/FRAME_PRE_SECOND);
            dt=0;
        }

        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            EngineRender();
            GuiRender();
        }

        glfwSwapBuffers(window);
    }
}

void glApplication::GuiRender(){
    static bool window_show=true;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::BeginMainMenuBar();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    ImGui::EndMainMenuBar();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void glApplication::EngineUpdate(double dt){
    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::FORWARD,dt);
    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::BACKWARD,dt);
    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::LEFT,dt);
    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::RIGHT,dt);
    if(glfwGetKey(window,GLFW_KEY_Q)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::UP,dt);
    if(glfwGetKey(window,GLFW_KEY_E)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::DOWN,dt);
}

void glApplication::EngineRender(){
    glm::mat4 model(1.0f);
    glm::mat4 view=MainCamera.GetViewMatrix();
    glm::mat4 proj=MainCamera.GetProjectionMatrix(WIDTH,HEIGHT);
    surface.defaultrender(Shaders,model,view,proj,MainCamera.Position);
}

void glApplication::cleanUp(){
    for(auto& shader:Shaders)shader.second.release();
    glfwTerminate();
}

void glApplication::framebufferResizeCallback(GLFWwindow* window,int width,int height){
    glApplication* app=(glApplication*)glfwGetWindowUserPointer(window);
    app->Width=width;
    app->Height=height;
    glViewport(0,0,width,height);
}

void glApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode){
    glApplication* app=(glApplication*)glfwGetWindowUserPointer(window);
    if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS){
        glfwSetWindowShouldClose(window,true);
    }
    if(key==GLFW_KEY_SPACE && action==GLFW_PRESS){
        if(app->MouseEnabled){
            app->firstMouse=true;
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
            app->MouseEnabled=false;
        }
        else{
            glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
            app->MouseEnabled=true;
        }
    }
}

void glApplication::cursorPosCallback(GLFWwindow* window, double xposIn, double yposIn){
    glApplication* app=(glApplication*)glfwGetWindowUserPointer(window);
    if(!app->MouseEnabled){
        static float lastX,lastY;
        float xpos=(float)xposIn,ypos=(float)yposIn;

        if(app->firstMouse){
            lastX=xpos;
            lastY=ypos;
            app->firstMouse=false;
        }

        float xoffset=xpos-lastX;
        float yoffset=lastY-ypos;

        lastX=xpos;
        lastY=ypos;

        app->MainCamera.ProcessMouseMovement(xoffset,yoffset);
    }
}
