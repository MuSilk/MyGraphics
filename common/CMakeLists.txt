file(GLOB common_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libcommon STATIC ${common_sources})
