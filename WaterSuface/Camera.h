#pragma once

class Camera {

private:
	glm::vec3 position;
	glm::vec3 target;
	glm::mat4 View;

public:
	Camera();
	~Camera();
};

