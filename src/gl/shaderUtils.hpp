#pragma once

#include <GL/glew.h>

#include <string>

namespace ShaderUtils {
    GLuint compile(std::string vertexShader, std::string fragmentShader);
} /* ShaderUtils */
