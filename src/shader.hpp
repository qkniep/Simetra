#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

#include <GLFW/glfw3.h>


class ShaderProgram {
	GLuint programID;


public:
	bool load(const char* vertexFilePath, const char* fragmentFilePath);
	void use();
	GLuint getProgramID();

private:
	static bool loadShaderFromFile(const GLuint shaderID, const char* filePath);
	static bool readShaderCodeFromFile(std::string& shaderCode, const char* filePath);
	static bool compileShader(const GLuint shaderID, const std::string& shaderCode);
	void linkProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID);
};

#endif  // SHADER_HPP
