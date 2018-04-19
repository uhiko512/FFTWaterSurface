#pragma once

#include "Camera.h"
#include "Skybox.h"
#include "Water.h"

struct CamMat {
	glm::mat4 view;
	glm::mat4 invView;
	glm::mat4 proj;
	glm::vec3 camPos;
};

class Renderer {

private:
	std::shared_ptr<ShaderManager> shaderManager;
	std::vector<std::shared_ptr<RenderObject>> renderObjects;
	std::map<std::string, std::shared_ptr<Camera>> cameras;
	
	CamMat camMat;

	GLuint framebuffer;
	GLuint waterRenderTarget[2];
	GLuint camUBO;
	
	glm::vec3 pos;
	glm::vec2 rot;
	glm::vec3 target;

public:
	Renderer(GLFWwindow* window);
	~Renderer();

	void exec();
	void keyEvent(glm::vec3 move);
	void mouseEvent(glm::vec2 rotation);

private:
	template<typename T>
	void addSceneObject();
	void addCamera(std::string name);
	void setupScene();
	void bulidSecne();
	void drawScene();
};


