#define GL_SILENCE_DEPRECATION

#include "shader.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


bool ShaderProgram::load(const char* vertexFilePath, const char* fragmentFilePath) {
	// Create shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	if (!loadShaderFromFile(vertexShaderID, vertexFilePath)) return false;
	if (!loadShaderFromFile(fragmentShaderID, fragmentFilePath)) return false;

	printf("Linking program\n");
	linkProgram(vertexShaderID, fragmentShaderID);
	return true;
}


void ShaderProgram::use() {
	glUseProgram(programID);
}


GLuint ShaderProgram::getProgramID() {
	return programID;
}


bool ShaderProgram::loadShaderFromFile(const GLuint shaderID, const char* filePath) {
	std::string shaderCode;
	if (!readShaderCodeFromFile(shaderCode, filePath)) return false;

	printf("Compiling shader: %s\n", filePath);
	if (!compileShader(shaderID, shaderCode)) return false;

	return true;
}


bool ShaderProgram::readShaderCodeFromFile(string& shaderCode, const char* filePath) {
	ifstream shaderStream(filePath, ios::in);
	if (!shaderStream.is_open()) {
		printf("ERROR (shader.cpp: loadShaderFromFile) Failed to open %s.\n", filePath);
		getchar();
		return false;
	}

	stringstream sstr;
	sstr << shaderStream.rdbuf();
	shaderCode = sstr.str();
	shaderStream.close();
	return true;
}


bool ShaderProgram::compileShader(const GLuint shaderID, const string& shaderCode) {
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, nullptr);
	glCompileShader(shaderID);

	// Check shader
	GLint result, infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		vector<char> shaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(shaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
		return false;
	}

	return true;
}


void ShaderProgram::linkProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID) {
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	// Check program
	GLint result, infoLogLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		vector<char> programErrorMessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, nullptr, &programErrorMessage[0]);
		printf("%s\n", &programErrorMessage[0]);
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	this->programID = programID;
}
