#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <GLFW/glfw3.h>


GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath);
bool loadShaderFromFile(const GLuint shaderID, const char* filePath);
bool readShaderCodeFromFile(std::string& shaderCode, const char* filePath);
bool compileShader(const GLuint shaderID, const std::string& shaderCode);
GLuint linkProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID);

#endif  // SHADER_HPP
