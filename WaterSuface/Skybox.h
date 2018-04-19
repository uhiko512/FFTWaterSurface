#pragma once
#include "RenderObject.h"

class Skybox : public RenderObject {

private:
	GLuint skyboxTex;

public:
	Skybox();
	~Skybox();

	void draw() override;
};

