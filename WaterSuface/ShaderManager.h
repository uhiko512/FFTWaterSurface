#pragma once

struct shaderCreateInfo {
	GLenum type;
	const char* filename;
};

class ShaderManager {

public:
	std::map<std::string, GLuint> shaderPrograms;

public:
	ShaderManager();
	~ShaderManager();

	GLuint createProgram(std::string shaderProgramName, std::vector<shaderCreateInfo> shaderInfos);
	GLuint _createProgram(std::vector<shaderCreateInfo> shaderInfos);
	GLuint createShader(shaderCreateInfo shaderInfo);
	std::string readShader(const std::string filename);
	void debugShader(GLuint shader);
};

