#include <iostream>
using namespace std;

#include <memory>
#include <application/glApplication.h>

int main(){
    std::unique_ptr<Application> app{new glApplication()};
    try {
        app->run();
    }
    catch(const std::exception& e){
        std::cerr<<e.what()<<std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}