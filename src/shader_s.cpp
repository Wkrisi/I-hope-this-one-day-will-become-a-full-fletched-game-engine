#include "shader_s.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

bool compileShader(GLuint shader, const char *source, const char *stageName)
{
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_TRUE) {
        return true;
    }

    char infoLog[1024]{};
    glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
    std::cerr << "Failed to compile " << stageName << " shader:\n" << infoLog << '\n';
    return false;
}

bool readTextFile(const char *path, std::string &contents)
{
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open shader file: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    contents = buffer.str();
    return true;
}

} // namespace

bool Shader::loadShaderProgramFromFile(const char *vertexShaderPath,
                                       const char *fragmentShaderPath)
{
    std::string vertexSource;
    std::string fragmentSource;
    if (!readTextFile(vertexShaderPath, vertexSource) ||
        !readTextFile(fragmentShaderPath, fragmentSource)) {
        return false;
    }

    return loadShaderProgramFromData(vertexSource.c_str(), fragmentSource.c_str());
}

bool Shader::loadShaderProgramFromData(const char *vertexShaderData,
                                       const char *fragmentShaderData)
{
    if (vertexShaderData == nullptr || fragmentShaderData == nullptr) {
        std::cerr << "Shader source cannot be null.\n";
        return false;
    }

    const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(vertexShader, vertexShaderData, "vertex") ||
        !compileShader(fragmentShader, fragmentShaderData, "fragment")) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char infoLog[1024]{};
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Failed to link shader program:\n" << infoLog << '\n';
        glDeleteProgram(program);
        return false;
    }

    clear();
    id = program;
    return true;
}

void Shader::bind()
{
    glUseProgram(id);
}

void Shader::use()
{
    bind();
}

void Shader::clear()
{
    if (id != 0) {
        glDeleteProgram(id);
        id = 0;
    }
}

GLint Shader::getUniformLocation(const char *name)
{
    if (id == 0) {
        return -1;
    }
    return glGetUniformLocation(id, name);
}

void Shader::setInt(const char *name, int value)
{
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                       glm::value_ptr(value));
}
