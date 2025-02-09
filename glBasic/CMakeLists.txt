file(GLOB glbasic_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libglbasic STATIC ${glbasic_sources})
