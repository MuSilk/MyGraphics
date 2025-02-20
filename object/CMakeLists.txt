file(GLOB object_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libobject STATIC ${object_sources})
