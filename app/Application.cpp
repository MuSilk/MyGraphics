#include "Application.h"

void Application::run(){
    initWindow();
    initGraphics();
    mainLoop();
    cleanUp();
}