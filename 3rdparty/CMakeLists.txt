file(GLOB imgui_sources CONFIGURE_DEPENDS 
    "imgui/*.cpp" 
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(libimgui STATIC ${imgui_sources})

target_include_directories(libimgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends)


