#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer(GLFWwindow* window) {
	shaderManager = std::make_shared<ShaderManager>();
	bulidSecne();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	
	glGenTextures(2, &waterRenderTarget[0]);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, waterRenderTarget[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, waterRenderTarget[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	GLuint depthRenderBuffer;
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	target = glm::vec3(0.0f, 0.0f, -1.0f);
	camMat.view = glm::lookAt(pos, target, glm::vec3(0.0f, 1.0f, 0.0f));
	camMat.invView = glm::inverse(glm::lookAt(glm::vec3(0.0f), target, glm::vec3(0.0f, 1.0f, 0.0f)));
	camMat.proj = glm::perspective(glm::radians(45.f), 1.7778f, 0.1f, 1000.f);

	glGenBuffers(1, &camUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, camUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CamMat), &camMat, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, camUBO);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CLIP_DISTANCE0);
}


Renderer::~Renderer() { }

void Renderer::exec() {
	camMat.camPos = pos;

	// Reflection
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, waterRenderTarget[0], 0);

	glm::vec3 rPos, rTarget;
	glm::vec3 N = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 P = glm::vec3(0.0f, 0.0f, -1.0f);
	rPos = pos + 2 * dot(pos - P, N) * -N;
	rTarget = target + 2 * dot(target - P, N) * -N;

	camMat.view = glm::lookAt(rPos, rPos + rTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	camMat.invView = glm::inverse(glm::lookAt(glm::vec3(0.0f), rTarget, glm::vec3(0.0f, 1.0f, 0.0f)));
	glBindBuffer(GL_UNIFORM_BUFFER, camUBO);
	GLvoid* p2 = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p2, &camMat, sizeof(camMat));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	renderObjects.at(1)->drawable = false;
	drawScene();

	// Refraction
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, waterRenderTarget[1], 0);

	camMat.view = glm::lookAt(pos, pos + target, glm::vec3(0.0f, 1.0f, 0.0f));
	camMat.invView = glm::inverse(glm::lookAt(glm::vec3(0.0f), target, glm::vec3(0.0f, 1.0f, 0.0f)));
	glBindBuffer(GL_UNIFORM_BUFFER, camUBO);
	GLvoid* p3 = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p3, &camMat, sizeof(camMat));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	drawScene();

	// Scene
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	camMat.view = glm::lookAt(pos, pos + target, glm::vec3(0.0f, 1.0f, 0.0f));
	camMat.invView = glm::inverse(glm::lookAt(glm::vec3(0.0f), target, glm::vec3(0.0f, 1.0f, 0.0f)));
	glBindBuffer(GL_UNIFORM_BUFFER, camUBO);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &camMat, sizeof(camMat));
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	renderObjects.at(1)->drawable = true;
	drawScene();
}

void Renderer::keyEvent(glm::vec3 move) {
	float speed = 1.0f;
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forward = glm::normalize(target);
	glm::vec3 right = glm::normalize(glm::cross(forward, up));

	pos += (right * move.x + forward * move.z) * speed;
	//std::cout << pos.x << std::endl;
	camMat.view = glm::lookAt(pos, pos + target, glm::vec3(0.0f, 1.0f, 0.0f));
	camMat.invView = glm::inverse(glm::lookAt(glm::vec3(0.0f), target, glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Renderer::mouseEvent(glm::vec2 rotation) {
	float speed = 20.0f;

	glm::mat4 Model;
	rot.x += rotation.x * speed;
	Model = glm::rotate(Model, glm::radians(rot.x), glm::vec3(0, 1, 0));
	if (abs(rot.y + rotation.y * speed) < 85.0f) {
		rot.y += rotation.y * speed;
		Model = glm::rotate(Model, glm::radians(rot.y), glm::vec3(1, 0, 0));
	}

	target = Model * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
	camMat.view = glm::lookAt(pos, pos + target, glm::vec3(0.0f, 1.0f, 0.0f));
	camMat.invView = glm::inverse(glm::lookAt(glm::vec3(0.0f), target, glm::vec3(0.0f, 1.0f, 0.0f)));
}

template<typename T>
void Renderer::addSceneObject() {
	renderObjects.push_back(std::make_shared<T>());
}

void Renderer::addCamera(std::string name) {
	cameras.insert(std::make_pair(name, std::make_shared<Camera>()));
}

void Renderer::setupScene() {
	addSceneObject<Skybox>();
	addSceneObject<Water>();
}

void Renderer::bulidSecne() {
	setupScene();
	
	for (auto obj : renderObjects) {
		for (auto info : obj->shaderProgramInfos) {
			obj->shaderPrograms.insert(std::make_pair(info.programName, shaderManager->createProgram(info.programName, info.shaderInfos)));
		}
	}
}

void Renderer::drawScene() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto obj : renderObjects) {
		if (obj->drawable) {
			obj->draw();
		}
	}
}
