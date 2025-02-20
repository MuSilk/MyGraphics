#pragma once

class Application{
public:
    void run();
protected:
    virtual void initWindow() = 0;
    virtual void initGraphics() = 0;
    virtual void mainLoop() = 0;
    virtual void cleanUp() = 0;
};

