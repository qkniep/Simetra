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


GLuint loadShaders(const char* vertexFilePath, const char* fragmentFilePath) {
	// Create shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	if (!loadShaderFromFile(vertexShaderID, vertexFilePath)) return 0;
	if (!loadShaderFromFile(fragmentShaderID, fragmentFilePath)) return 0;

	printf("Linking program\n");
	return linkProgram(vertexShaderID, fragmentShaderID);
}


bool loadShaderFromFile(const GLuint shaderID, const char* filePath) {
	std::string shaderCode;
	if (!readShaderCodeFromFile(shaderCode, filePath)) return false;

	printf("Compiling shader: %s\n", filePath);
	if (!compileShader(shaderID, shaderCode)) return false;

	return true;
}


bool readShaderCodeFromFile(string& shaderCode, const char* filePath) {
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


bool compileShader(const GLuint shaderID, const string& shaderCode) {
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


GLuint linkProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID) {
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

	return programID;
}
