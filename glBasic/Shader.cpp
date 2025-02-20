#include "Shader.h"

void Shader::compile(const char* vertexSource, const char* geometrySource, const char* fragmentSource) {

    ID = glCreateProgram();

    unsigned int vertex, fragment, geometry;
    int success;
    char infolog[512];
    if (vertexSource != NULL) {
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexSource, NULL);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infolog);
            std::cerr << "ERROR::SHADERVERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
        }
        glAttachShader(ID, vertex);
        glDeleteShader(vertex);
    }
    if (fragmentSource != NULL) {
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentSource, NULL);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infolog);
            std::cerr << "ERROR::SHADERFRAGMENT::COMPILATION_FAILED\n" << infolog << std::endl;
        }
        glAttachShader(ID, fragment);
        glDeleteShader(fragment);
    }
    if (geometrySource != NULL) {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &geometrySource, NULL);
        glCompileShader(geometry);

        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(geometry, 512, NULL, infolog);
            std::cerr << "ERROR::SHADERGEOMETRY::COMPILATION_FAILED\n" << infolog << std::endl;
        }
        glAttachShader(ID, geometry);
        glDeleteShader(geometry);
    }

    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infolog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infolog << std::endl;
    }
}

void Shader::compileFromFile(const char* vertexfile, const char* geometryfile, const char* fragmentfile) {
    std::string vert, frag;
    LoadFile(vertexfile, vert); LoadFile(fragmentfile, frag);
    if (geometryfile == nullptr) {
        compile(vert.c_str(), nullptr, frag.c_str());
    }
    else {
        std::string geog; LoadFile(geometryfile, geog);
        compile(vert.c_str(), geog.c_str(), frag.c_str());
    }
}

Shader Shader::Shader_2drgb(){
    const char* vert =
		"#version 330 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"

		"layout (location = 0) out vec3 fragColor;\n"
		"void main(){\n"
		"    gl_Position = vec4(aPos.x,aPos.y,0.0,1.0);\n"
		"    fragColor = aColor;\n"
		"}\0";

	const char* frag =
		"#version 330 core\n"
		"layout(location=0) in vec3 fragColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(fragColor, 1.0f);\n"
		"}\0";
	Shader shader;
	shader.compile(vert, nullptr, frag);
	return shader;
}

Shader Shader::Shader_3drgb(){
    const char* vert =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec3 aColor;\n"

		"layout (location = 0) out vec3 fragColor;\n"
		"void main(){\n"
		"    gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
		"    fragColor = aColor;\n"
		"}\0";

	const char* frag =
		"#version 330 core\n"
		"layout(location=0) in vec3 fragColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(fragColor, 1.0f);\n"
		"}\0";
	Shader shader;
	shader.compile(vert, nullptr, frag);
	return shader;
}
Shader Shader::Shader_3d_single_color() {
	const char* vert =
		"#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"

		"uniform vec3 aColor;"
		"uniform mat4 mvp;"

		"layout (location = 0) out vec3 fragColor;\n"
		"void main(){\n"
		"    gl_Position = mvp*vec4(aPos,1.0);\n"
		"    fragColor = aColor;\n"
		"}\0";

	const char* frag =
		"#version 330 core\n"
		"layout(location=0) in vec3 fragColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(fragColor, 1.0f);\n"
		"}\0";
	Shader shader;
	shader.compile(vert, nullptr, frag);
	return shader;
}

Shader Shader::Shader_v3c3mvp(){
	const char* vert =
		"#version 330 core\n"
		"layout (location = 0) in vec3 V;\n"
		"layout (location = 1) in vec3 C;\n"
		"uniform mat4 mvp;"
		"layout (location = 0) out vec3 fragColor;\n"
		"void main(){\n"
		"    gl_Position = mvp*vec4(V,1.0);\n"
		"    fragColor = C;\n"
		"}\0";

	const char* frag =
		"#version 330 core\n"
		"layout(location=0) in vec3 fragColor;\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(fragColor, 1.0f);\n"
		"}\0";

	Shader shader;
	shader.compile(vert, nullptr, frag);
	return shader;
}