file(GLOB tinyxml2_sources CONFIGURE_DEPENDS "*.cpp")

add_library(libtinyxml2 STATIC ${tinyxml2_sources})
