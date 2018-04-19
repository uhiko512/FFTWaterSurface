#pragma once

#include "ShaderManager.h"

struct shaderProgramCreateInfo {
	std::vector<shaderCreateInfo> shaderInfos;
	std::string programName;
};

class RenderObject {

public:
	std::vector<shaderProgramCreateInfo> shaderProgramInfos;
	std::map<std::string, GLuint> shaderPrograms;
	bool drawable = true;

protected:
	GLuint VAO;

public:
	RenderObject();
	~RenderObject();

	virtual void draw() = 0;
};

