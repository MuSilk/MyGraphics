file(GLOB glad_sources CONFIGURE_DEPENDS "*.c")

include_directories("./glad")
add_library(libglad STATIC ${glad_sources})
