#include "glApplication.h"

#include <stdexcept>
#include <vector>
#include <filesystem>

#include <imgui/imgui_internal.h>
#include <nfd.h>

#include <glBasic/ShaderManager.h>

#define FRAME_PRE_SECOND 120

static inline ImVec2 glm2imgui(glm::vec2 v){
    return ImVec2(v.x,v.y);
}

static inline glm::vec2 imgui2glm(ImVec2 v){
    return glm::vec2(v.x,v.y);
}

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
    glfwSetMouseButtonCallback(window,mouseButtonCallback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("failed to initalize GLAD");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io=&ImGui::GetIO();(void)io;

    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
    glViewport(0,0,Width,Height);
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT,GL_FILL);

    glPointSize(5);

    MainCamera.Position=glm::vec3(-1.0f,1.0f,10.0f);
}

#include <surface/Curve.h>
#include <surface/Mesh.h>
#include <surface/ScreenVT.h>
#include <shaders/rc.h>
#include <raytrace/RayTrace.h>

#include "glBasic/glDynamicObject.h"

void glApplication::createShaders(){

}

RayTrace ray_trace;
void glApplication::createDataBuffer(){

    // ray_trace.init(Width,Height);
    Mesh* cube = new Mesh(Mesh::evalCube());
    cube->init();
    DataObjects["cube"]=cube;

    Curve* circle = new Curve(Curve::evalCircle(1.0f,100)); 
    circle->init();
    DataObjects["circle"]=circle;

    Mesh* solidcircle=new Mesh(Mesh::evalCircle(1.0f,100));
    solidcircle->init();
    DataObjects["solid_circle"]=solidcircle;

    Mesh* sphere=new Mesh(Mesh::evalSphere(1.0f,100));
    sphere->init();
    DataObjects["sphere"]=sphere;

    Mesh* quad=new Mesh(Mesh::evalQuad());
    quad->init();
    DataObjects["quad"]=quad;
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
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            EngineRender();
            GuiRender();
        }

        glfwSwapBuffers(window);
    }
}

void glApplication::GuiRender(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::BeginMainMenuBar();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    ImGui::EndMainMenuBar();

    ImGuiID dockspaceID=ImGui::GetID("##ui.dock_space");
    const char* UI_DOCK_WINDOW  = "##ui.dock_window";
    const char* UI_PROJECT_BOX  = u8"project##ui.project";
    const char* UI_PROPERTY_BOX = u8"property##ui.property";
    const char* UI_TOOL_BOX     = u8"tools##ui.tools";
    const char* UI_RESOURCE_BOX  = u8"resource##ui.resource";
    const char* UI_LOG_BOX      = u8"log##ui.log";
    const char* UI_VIEW_BOX     = u8"##ui.view";

    const ImGuiViewport* viewport=ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    int windowFlags = 
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBackground | 
        ImGuiWindowFlags_MenuBar | 
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    ImGui::Begin(UI_DOCK_WINDOW,0,windowFlags);
    ImGui::PopStyleVar(3);

    if(ImGui::BeginMenuBar()){
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {  }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {  }
            if (ImGui::MenuItem("Close", "Ctrl+W")) {  }
            if (ImGui::BeginMenu("Import")){
                if(ImGui::MenuItem("model","*.obj")){
                    char* path=nullptr;
                    if(NFD_OpenDialog("obj",NULL,&path)==NFD_OKAY){
                        Mesh* mesh=new Mesh(Mesh::evalModel(path));
                        mesh->init();
                        std::filesystem::path p(path);
                        auto filename=p.filename().string();
                        std::string name=filename;
                        if(DataObjects.find(name)!=DataObjects.end()){
                            for(size_t i=1;;i++){
                                name=filename+"("+std::to_string(i)+")";
                                if(DataObjects.find(name)==DataObjects.end())break;
                            }
                        }
                        DataObjects[name]=mesh;
                        free(path);
                    }
                }
                if(ImGui::MenuItem("Texture","*.png/*.jpg")){
                    char* path=nullptr;
                    if(NFD_OpenDialog("png;jpg",NULL,&path)==NFD_OKAY){
                        Texture2D texture;
                        texture.Generate(path);
                        std::filesystem::path p(path);
                        auto filename=p.filename().string();
                        std::string name=filename;
                        if(Textures.find(name)!=Textures.end()){
                            for(size_t i=1;;i++){
                                name=filename+"("+std::to_string(i)+")";
                                if(Textures.find(name)==Textures.end())break;
                            }
                        }
                        Textures[name]=texture;
                        free(path);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            bool enabled = true; 
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, enabled)) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, enabled)) {}

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::RadioButton("Common", renderOption == RENDEROPTION_COMMON)) { 
            renderOption = RENDEROPTION_COMMON;
        }

        if (ImGui::RadioButton("Phone", renderOption == RENDEROPTION_PHONE)) { 
            renderOption = RENDEROPTION_PHONE;
        }

        ImGui::EndMenuBar();
    }

    if(io->ConfigFlags & ImGuiConfigFlags_DockingEnable){
        if(!ImGui::DockBuilderGetNode(dockspaceID)){
            ImGui::DockBuilderRemoveNode(dockspaceID);
            ImGuiID root=ImGui::DockBuilderAddNode(dockspaceID,ImGuiDockNodeFlags_None); 

            ImGui::DockBuilderSetNodePos(root,{0.0f,0.0f});
            ImGui::DockBuilderSetNodeSize(root,ImGui::GetWindowSize());

            ImGuiID leftNode = ImGui::DockBuilderSplitNode(root,ImGuiDir_Left,0.25f,nullptr,&root);

            ImGuiID rightNode = ImGui::DockBuilderSplitNode(root, ImGuiDir_Right, 0.25f / 0.75f, nullptr, &root);

            ImGuiID bottomNode = ImGui::DockBuilderSplitNode(root, ImGuiDir_Down, 0.25f, nullptr, &root);

            ImGuiID rightTopNode, rightBottomNode;
            ImGui::DockBuilderSplitNode(rightNode, ImGuiDir_Up, 0.3f, &rightTopNode, &rightBottomNode);

            ImGui::DockBuilderGetNode(root)->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar;

            ImGui::DockBuilderDockWindow(UI_TOOL_BOX, leftNode); 
            ImGui::DockBuilderDockWindow(UI_PROPERTY_BOX, rightBottomNode);  
            ImGui::DockBuilderDockWindow(UI_PROJECT_BOX, rightTopNode);           
            ImGui::DockBuilderDockWindow(UI_RESOURCE_BOX, rightTopNode);      

            ImGui::DockBuilderDockWindow(UI_LOG_BOX, bottomNode);
  
            ImGui::DockBuilderDockWindow(UI_VIEW_BOX, root);

            ImGui::DockBuilderFinish(dockspaceID);

            ImGui::SetWindowFocus(UI_VIEW_BOX);
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }
    ImGui::End();

    if(ImGui::Begin(UI_PROJECT_BOX)){
        if(ImGui::CollapsingHeader("MainSence",ImGuiTreeNodeFlags_DefaultOpen)){
            for(auto& [id,obj]:MainScene.Objects){
                if(ImGui::Selectable(obj->name.c_str(),&obj->selected)){
                    if(obj->selected)MainScene.selectObject(id);
                    else MainScene.deselectObjcet(id);
                }
            }
        }
    }
    ImGui::End();

    if(ImGui::Begin(UI_TOOL_BOX)){
        ImGui::LabelText("label","test");
        ImGui::Button("click me and get nothing");
    }
    ImGui::End();

    if(ImGui::Begin(UI_PROPERTY_BOX)){
        if(MainScene.lastSelectObject!=-1){
            auto obj=MainScene.Objects[MainScene.lastSelectObject];

            ImGui::LabelText("Name",obj->name.c_str());
            ImGui::LabelText("Id","%d",obj->id);

            if(obj->objectType()==ObjectType::COMMON){
                if(ImGui::CollapsingHeader("Transform",ImGuiTreeNodeFlags_DefaultOpen)){
                    ImGui::SeparatorText("Location");
                    ImGui::InputFloat("X##Location.x", &obj->position.x);
                    ImGui::InputFloat("Y##Location.y", &obj->position.y);
                    ImGui::InputFloat("Z##Location.z", &obj->position.z);

                    ImGui::SeparatorText("Rotation");
                    ImGui::InputFloat("W##Rotation.w", &obj->rotation.w);
                    ImGui::InputFloat("X##Rotation.x", &obj->rotation.x);
                    ImGui::InputFloat("Y##Rotation.y", &obj->rotation.y);
                    ImGui::InputFloat("Z##Rotation.z", &obj->rotation.z);

                    ImGui::SeparatorText("Scale");
                    ImGui::InputFloat("X##Scale.x", &obj->scale.x);
                    ImGui::InputFloat("Y##Scale.y", &obj->scale.y);
                    ImGui::InputFloat("Z##Scale.z", &obj->scale.z);
                }
            }
            else if(obj->objectType()==ObjectType::PHONE){
                if(ImGui::CollapsingHeader("Transform",ImGuiTreeNodeFlags_DefaultOpen)){
                    ImGui::SeparatorText("Location");
                    ImGui::InputFloat("X##Location.x", &obj->position.x);
                    ImGui::InputFloat("Y##Location.y", &obj->position.y);
                    ImGui::InputFloat("Z##Location.z", &obj->position.z);

                    ImGui::SeparatorText("Rotation");
                    ImGui::InputFloat("W##Rotation.w", &obj->rotation.w);
                    ImGui::InputFloat("X##Rotation.x", &obj->rotation.x);
                    ImGui::InputFloat("Y##Rotation.y", &obj->rotation.y);
                    ImGui::InputFloat("Z##Rotation.z", &obj->rotation.z);

                    ImGui::SeparatorText("Scale");
                    ImGui::InputFloat("X##Scale.x", &obj->scale.x);
                    ImGui::InputFloat("Y##Scale.y", &obj->scale.y);
                    ImGui::InputFloat("Z##Scale.z", &obj->scale.z);
                }
                auto phone_ptr=(PhoneObject*)obj;
                if(ImGui::CollapsingHeader("Surface",ImGuiTreeNodeFlags_DefaultOpen)){
                    const char* type[] = { "SingleColor", "Texture"};
                    static int diffuse_selected = ((int)phone_ptr->diffuse->surfaceType())-1; 
                    static int specular_selected = ((int)phone_ptr->specular->surfaceType())-1;

                    ImGui::SeparatorText("Diffuse");
                    if (ImGui::Combo("Type##diffuse.type", &diffuse_selected, type, IM_ARRAYSIZE(type)));
                    switch (diffuse_selected){
                        case 0:{
                            ImGui::ColorEdit3("Color##diffuse.color",(float*)&(((SurfaceColor*)(phone_ptr->diffuse))->color));
                            break;
                        }
                        case 1:{
                            std::vector<const char*> texture_list={"None"};
                            for(auto& [name,texture]:Textures)texture_list.push_back(name.c_str());
                            static int selected=0;
                            // string texture_name=((SurfaceTexture*)(phone_ptr->diffuse))->TextureName;
                            string texture_name="";
                            for(int i=1;i<texture_list.size();i++){
                                if(texture_list[i]==texture_name){
                                    selected=i;
                                    break;
                                }
                            }
                            if (ImGui::Combo("Texture##diffuse.texture", &selected, texture_list.data(), texture_list.size()));
                            break;
                        }
                    }
                    
                    ImGui::SeparatorText("Specular");
                    if (ImGui::Combo("Type##specular.type", &specular_selected, type, IM_ARRAYSIZE(type)));
                    ImGui::ColorEdit3("Color##specular.color",(float*)&(((SurfaceColor*)(phone_ptr->specular))->color));
                    
                    ImGui::SeparatorText("Shininess");
                    ImGui::InputFloat("shininess",&(phone_ptr->shininess));
                }
            }
            else if(obj->objectType()==ObjectType::LIGHT){
                if(ImGui::CollapsingHeader("Transform",ImGuiTreeNodeFlags_DefaultOpen)){
                    ImGui::SeparatorText("Location");
                    ImGui::InputFloat("X##Location.x", &obj->position.x);
                    ImGui::InputFloat("Y##Location.y", &obj->position.y);
                    ImGui::InputFloat("Z##Location.z", &obj->position.z);
                }

                if(ImGui::CollapsingHeader("Surface",ImGuiTreeNodeFlags_DefaultOpen)){
                    if(ImGui::ColorEdit3("Color",(float*)&(((Light*)obj)->color)));
                    if(ImGui::InputFloat("Constant",&(((Light*)obj)->constant)));
                    if(ImGui::InputFloat("Linear",&(((Light*)obj)->linear)));
                    if(ImGui::InputFloat("Quadratic",&(((Light*)obj)->quadratic)));
                }
            }
            
        }
    }
    ImGui::End();

    if(ImGui::Begin(UI_RESOURCE_BOX)){
        if(ImGui::CollapsingHeader("Models",ImGuiTreeNodeFlags_DefaultOpen)){
            for(auto& [name,obj]:DataObjects){
                auto tag=name+"##RESOURCE";
                if(ImGui::Selectable(tag.c_str())){}
                if(ImGui::IsItemHovered()){
                    if(ImGui::BeginDragDropSource()){
                        ImGui::SetDragDropPayload("MODEL_NAME",name.c_str(),(name.length() + 1) * sizeof(char));
                        ImGui::Text(name.c_str());
                        ImGui::EndDragDropSource();
                    }
                }
            }
        }
        if(ImGui::CollapsingHeader("Textures",ImGuiTreeNodeFlags_DefaultOpen)){
            for(auto& [name,texture]:Textures){
                if(ImGui::Selectable(name.c_str())){

                }
            }
        }
    }
    ImGui::End();

    if(ImGui::Begin(UI_LOG_BOX)){
        ImGui::LabelText("label","test");
        ImGui::Button("click me and get nothing");
    }
    ImGui::End();
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);            
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); 
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);              

    if(ImGui::Begin(UI_VIEW_BOX,0,ImGuiWindowFlags_NoBackground)){
        ViewBoxPos  = imgui2glm(ImGui::GetWindowPos());
        ViewBoxSize = imgui2glm(ImGui::GetWindowSize());

        ImGui::Dummy(ImGui::GetWindowSize());

        if (ImGui::BeginDragDropTarget()){
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MODEL_NAME")){
                PhoneObject obj=PhoneObject::evalMeshObject(
                    MainCamera.GetPointPosition(mousePos,20.0f,Width,Height),
                    DataObjects[(char*)payload->Data],
                    MainCamera,Width,Height
                );
                MainScene.addObject(obj);
            }
            ImGui::EndDragDropTarget();
        }

        if(ImGui::BeginPopupContextWindow()){
            if(ImGui::MenuItem("Create Cube",NULL)){
                PhoneObject obj=PhoneObject::evalMeshObject(
                    MainCamera.GetPointPosition(mouseRightClickMenuPos,20.0f,Width,Height),
                    DataObjects["cube"],
                    MainCamera,Width,Height
                );
                MainScene.addObject(obj);
            }
            if(ImGui::MenuItem("Create Light",NULL)){
                Light obj; 
                obj.position=MainCamera.GetPointPosition(mouseRightClickMenuPos,20.0f,Width,Height);
                obj.scale=glm::vec3(0.1f,0.1f,0.1f);
                obj.rotation=glm::quat(glm::vec3(0.0f,0.0f,0.0f));
                obj.dataSurface=DataObjects["circle"];
                obj.clickRegion=DataObjects["solid_circle"];

                obj.render=Scene::defaultrender(&MainCamera,&Width,&Height);
                obj.render_margin=Scene::defaultrender_region(&MainCamera,&Width,&Height);

                obj.update=[&](RenderObject& thisobj){
                    auto z=glm::normalize(MainCamera.Position-thisobj.position);
                    auto x=glm::normalize(glm::cross(glm::vec3(0.0f,1.0f,0.0f),z));
                    auto y=glm::normalize(glm::cross(z,x));
                    glm::mat3 rotate={x,y,z};
                    thisobj.rotation=glm::quat(rotate);
                };
                
                obj.dataSurface->name="Light";
                MainScene.addObject(obj);
                obj.dataSurface->name="Circle";
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void glApplication::EngineUpdate(double dt){
    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::FORWARD,dt),ray_trace.initrender();
    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::BACKWARD,dt),ray_trace.initrender();
    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::LEFT,dt),ray_trace.initrender();
    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::RIGHT,dt),ray_trace.initrender();
    if(glfwGetKey(window,GLFW_KEY_Q)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::UP,dt),ray_trace.initrender();
    if(glfwGetKey(window,GLFW_KEY_E)==GLFW_PRESS)
        MainCamera.ProcessKeyboard(Camera_Movement::DOWN,dt),ray_trace.initrender();

    MainScene.update();
}

void glApplication::EngineRender(){
    glm::mat4 model(1.0f);
    glm::mat4 view=MainCamera.GetViewMatrix();
    glm::mat4 proj=MainCamera.GetProjectionMatrix(WIDTH,HEIGHT);

    switch (renderOption){
        case RENDEROPTION_COMMON:
            MainScene.render();
            MainScene.render_select();
            break;
        case RENDEROPTION_PHONE:
            MainScene.render_phone();
        default:
            break;
        }

    // ray_trace.render(MainCamera,Width,Height);
}

void glApplication::cleanUp(){
    ShaderManagerv1::cleanUp();
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

void glApplication::dragSelectObjects(glm::vec2 mouseMovement){
    mouseMovement.x/=Width;
    mouseMovement.y/=Height;

    auto grid=MainCamera.GetGrid(Width,Height);
    auto center=MainScene.getObjectsSelectCenter();
    auto distance=glm::dot(center-MainCamera.Position,MainCamera.Front);
    glm::vec3 translation=MainCamera.Right*mouseMovement.x*distance*grid.x+MainCamera.Up*mouseMovement.y*distance*grid.y;
    
    if(ImRect(glm2imgui(ViewBoxPos),glm2imgui(ViewBoxPos+ViewBoxSize)).Contains(glm2imgui(mouseLeftClickPos))){
        MainScene.dragSelectObjects(translation);
    }
}

void glApplication::cursorPosCallback(GLFWwindow* window, double xposIn, double yposIn){
    glApplication* app=(glApplication*)glfwGetWindowUserPointer(window);

    float xpos=(float)xposIn,ypos=(float)yposIn;

    if(app->firstMouse){
        app->mousePos.x=xpos;
        app->mousePos.y=ypos;
        app->firstMouse=false;
    }

    float xoffset=xpos-app->mousePos.x;
    float yoffset=app->mousePos.y-ypos;

    app->mousePos.x=xpos;
    app->mousePos.y=ypos;

    if(!app->MouseEnabled){
        app->MainCamera.ProcessMouseMovement(xoffset,yoffset);
        ray_trace.initrender();
    }
    else{
        if(app->MouseStatus[GLFW_MOUSE_BUTTON_LEFT]){
            app->dragSelectObjects(glm::vec3(xoffset,yoffset,0.0f));
        }
    }
}

void glApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mode){
    glApplication* app=(glApplication*)glfwGetWindowUserPointer(window);

    switch (action){
    case GLFW_PRESS:
        app->MouseStatus[button]=true;
        break;
    case GLFW_RELEASE:
        app->MouseStatus[button]=false;
        break;
    default:
        break;
    }

    if(button==GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS && app->MouseEnabled){
        app->mouseRightClickMenuPos=app->mousePos;
    }
    if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS && app->MouseEnabled){
        app->mouseLeftClickPos=app->mousePos;
    }
    if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS){
        if(ImRect(glm2imgui(app->ViewBoxPos),glm2imgui(app->ViewBoxPos+app->ViewBoxSize)).Contains(glm2imgui(app->mousePos))){
            auto dir=app->MainCamera.GetDirection(app->mousePos,app->Width,app->Height);
            uint32_t idt=-1;
            float t=std::numeric_limits<float>::max();
            if(app->MainScene.intersect(app->MainCamera.Position,dir,0.1f,t,idt)&&!app->MainScene.Objects[idt]->selected){
                app->MainScene.clearSelcectObjects();
                app->MainScene.selectObject(idt);
            }
            else if(idt==-1){
                app->MainScene.clearSelcectObjects();
            }
        }
    }
}