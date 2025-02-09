file(GLOB app_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libapp STATIC ${app_sources})
