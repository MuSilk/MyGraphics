file(GLOB raytrace_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libraytrace STATIC ${raytrace_sources})
