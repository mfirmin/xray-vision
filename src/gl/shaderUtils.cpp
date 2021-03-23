#include "shaderUtils.hpp"

#include <iostream>
#include <vector>

GLuint ShaderUtils::compile(std::string vs, std::string fs) {
    const GLchar* vertexShaderSource[] = { vs.c_str() };
    const GLchar* fragmentShaderSource[] = { fs.c_str() };

    GLint success = 0;

    // Create shaders...
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, static_cast<const GLchar**>(vertexShaderSource), nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorLog[0]);
        glDeleteShader(vertexShader);

        for (auto it : errorLog) {
            std::cout << it;
        }
        return 0;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, static_cast<const GLchar**>(fragmentShaderSource), nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorLog[0]);
        glDeleteShader(fragmentShader);

        for (auto it : errorLog) {
            std::cout << it;
        }
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        for (auto it : infoLog) {
            std::cout << it;
        }

        glDeleteProgram(program);
        return 0;
    }

    return program;
}
