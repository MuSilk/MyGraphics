file(GLOB SHADER_FILES CONFIGURE_DEPENDS "*.vert" "*.frag" "*.geom")

set(EMBED_HEADER rc.h)
set(EMBED_SOURCE rc.cpp)

function(GENERATE_EMBED_FILE)
    file(WRITE ${EMBED_HEADER} "")
    file(WRITE ${EMBED_SOURCE} "#include \"rc.h\"\n")

    foreach(FILE ${SHADER_FILES})
        file(SIZE ${FILE} FILE_SIZE)
        get_filename_component(FILE_NAME ${FILE} NAME)

        string(MAKE_C_IDENTIFIER ${FILE_NAME} token)
        string(TOUPPER ${token} token)
        file(READ ${FILE} filedata HEX)
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        file(APPEND ${EMBED_SOURCE}
            "const unsigned char ${token}_DATA[] = {\n${filedata}0x00\n};\n"
            "const unsigned long ${token}_SIZE   = ${FILE_SIZE};\n")
        file(APPEND ${EMBED_HEADER}
            "extern const unsigned char ${token}_DATA[];\n"
            "extern const unsigned long ${token}_SIZE;\n")

    endforeach()
endfunction()

GENERATE_EMBED_FILE()

# 创建一个库或者可执行文件，并链接生成的资源
file(GLOB EMBED_SOURCE CONFIGURE_DEPENDS "*.cpp")
add_library(libshaders STATIC ${EMBED_SOURCE})