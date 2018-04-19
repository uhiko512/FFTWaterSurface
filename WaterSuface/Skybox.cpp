#include "stdafx.h"
#include "Skybox.h"


Skybox::Skybox() {
	// Shader
	shaderProgramCreateInfo skybox;
	skybox.shaderInfos = {
		{ GL_VERTEX_SHADER, "SkyboxVS.glsl" },
		{ GL_FRAGMENT_SHADER, "SkyboxPS.glsl" }
	};
	skybox.programName = "Skybox";
	shaderProgramInfos.push_back(skybox);

	// Vertex
	std::vector<GLfloat> vertices = std::vector<GLfloat>{
		-1.0f, -1.0f,
		1.0f, -1.0f,
		1.0f,  1.0f,

		-1.0f, -1.0f,
		1.0f,  1.0f,
		-1.0f,  1.0f
	};

	glGenVertexArrays(1, &VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	// Texture
	std::string root = "Resource/Cubemap/";
	std::string name = "bloody-heresy";
	
	skyboxTex = SOIL_load_OGL_cubemap(
			(root + name + "_ft.tga").c_str(),
			(root + name + "_bk.tga").c_str(),
			(root + name + "_up.tga").c_str(),
			(root + name + "_dn.tga").c_str(),
			(root + name + "_rt.tga").c_str(),
			(root + name + "_lf.tga").c_str(),
			SOIL_LOAD_RGBA,
			8,
			SOIL_FLAG_MIPMAPS
		);
}


Skybox::~Skybox() { }

void Skybox::draw() {
	glUseProgram(shaderPrograms["Skybox"]);

	glm::mat4 Model;
	Model = glm::rotate(Model, glm::radians((float) glfwGetTime() * 0.7f), glm::vec3(0, 1, 0));
	GLuint modelLoc = glGetUniformLocation(shaderPrograms["Skybox"], "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Model));

	glDepthMask(GL_FALSE);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}
