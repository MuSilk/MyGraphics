file(GLOB surface_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libsurface STATIC ${surface_sources})
