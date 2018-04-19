#include "stdafx.h"
#include "Camera.h"


Camera::Camera() {
	position = glm::vec3(0.0f);
	target = glm::vec3(0.0f);
	View = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}


Camera::~Camera() { }
