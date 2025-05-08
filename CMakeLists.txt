cmake_minimum_required(VERSION 3.5.0)
project(MyEngine VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

include_directories(3rdparty)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(3rdparty/imgui)
include_directories(3rdparty/imgui/backends)
include_directories(3rdparty/nativefiledialog/src/include)
include_directories(3rdparty/tinyxml2)

add_subdirectory(3rdparty/glad)
target_link_libraries(${PROJECT_NAME} libglad)

target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/3rdparty/GLFW/lib/glfw3.lib")

target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/3rdparty/nativefiledialog/build/lib/Debug/x64/nfd_d.lib")

add_subdirectory(3rdparty/tinyxml2)
target_link_libraries(${PROJECT_NAME} libtinyxml2)

add_subdirectory(3rdparty)
target_link_libraries(${PROJECT_NAME} libimgui)

add_subdirectory(application)
target_link_libraries(${PROJECT_NAME} libapp)

add_subdirectory(glBasic)
target_link_libraries(${PROJECT_NAME} libglbasic)

add_subdirectory(Surface)
target_link_libraries(${PROJECT_NAME} libsurface)

add_subdirectory(shaders)
target_link_libraries(${PROJECT_NAME} libshaders)

add_subdirectory(object)
target_link_libraries(${PROJECT_NAME} libobject)

add_subdirectory(raytrace)
target_link_libraries(${PROJECT_NAME} libraytrace)