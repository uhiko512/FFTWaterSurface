#include "stdafx.h"
#include "ShaderManager.h"

ShaderManager::ShaderManager() { }

ShaderManager::~ShaderManager() { }

GLuint ShaderManager::createProgram(std::string shaderProgramName, std::vector<shaderCreateInfo> shaderInfos) {
	GLuint program;
	
	if (shaderPrograms.find(shaderProgramName) == shaderPrograms.end()) {
		program = _createProgram(shaderInfos);
		shaderPrograms.insert(std::pair<std::string, GLuint>(shaderProgramName, program));
	} else {
		program = shaderPrograms.at(shaderProgramName);
	}

	return program;
}

GLuint ShaderManager::_createProgram(std::vector<shaderCreateInfo> shaderInfos) {
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();

	std::vector<GLuint> shaders;
	for (auto info : shaderInfos) {
		GLuint shader = createShader(info);
		shaders.push_back(shader);
		glAttachShader(shaderProgram, shader);
	}
	glLinkProgram(shaderProgram);

	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		std::cout << "Fail to create shader program.\n";

		std::vector<GLchar> infoLog(512);
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog.data());
		std::cout << infoLog.data();

		glDeleteProgram(shaderProgram);
	}

	for (auto shader : shaders) {
		glDeleteShader(shader);
	}

	return shaderProgram;
}

GLuint ShaderManager::createShader(shaderCreateInfo shaderInfo) {
	GLuint shader;
	shader = glCreateShader(shaderInfo.type);

	std::string shaderSource;
	shaderSource = readShader(shaderInfo.filename);
	const GLchar* source = shaderSource.c_str();
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	debugShader(shader);
	
	return shader;
}

std::string ShaderManager::readShader(const std::string filename) {
	std::string glslCode;
	std::ifstream glslFile;
	
	glslFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	try {
		glslFile.open("Resource/Shader/" + filename, std::ios::in | std::ios::binary);
		GLint fSize = (GLuint) glslFile.tellg();
		std::stringstream glslStream;
		glslStream << glslFile.rdbuf();
		glslFile.close();
		glslCode = glslStream.str();
	} catch (std::ifstream::failure e) {
		std::cout << e.what() << std::endl;
	}

	return glslCode;
}

void ShaderManager::debugShader(GLuint shader) {
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE) {
		std::cout << "Fail to create shader.\n";

		std::vector<GLchar> errorLog(512);
		glGetShaderInfoLog(shader, 512, nullptr, errorLog.data());
		std::cout << errorLog.data();

		glDeleteShader(shader);
	}
}
