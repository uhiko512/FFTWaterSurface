#include "stdafx.h"
#include "Water.h"


Water::Water() {
	isDrawInit = false;
	texSize = 256;
	ifftCount = 5;

	// Shader
	shaderProgramCreateInfo waterRendering;
	waterRendering.shaderInfos = {
		{ GL_VERTEX_SHADER, "WaterRenderingVS.glsl" },
		{ GL_TESS_CONTROL_SHADER, "WaterRenderingTCS.glsl" },
		{ GL_TESS_EVALUATION_SHADER, "WaterRenderingTES.glsl" },
		{ GL_FRAGMENT_SHADER, "WaterRenderingPS.glsl" }
	};
	waterRendering.programName = "WaterRendering";
	shaderProgramInfos.push_back(waterRendering);

	shaderProgramCreateInfo waterSurfaceHeightInit;
	waterSurfaceHeightInit.shaderInfos = {
		{ GL_COMPUTE_SHADER, "WaterSurfaceHeightInitCS.glsl" }
	};
	waterSurfaceHeightInit.programName = "WaterSurfaceHeightInit";
	shaderProgramInfos.push_back(waterSurfaceHeightInit);

	shaderProgramCreateInfo waterSurfaceButterfly;
	waterSurfaceButterfly.shaderInfos = {
		{ GL_COMPUTE_SHADER, "WaterSurfaceButterflyCS.glsl" }
	};
	waterSurfaceButterfly.programName = "WaterSurfaceButterfly";
	shaderProgramInfos.push_back(waterSurfaceButterfly);

	shaderProgramCreateInfo waterSurfaceHeight;
	waterSurfaceHeight.shaderInfos = {
		{ GL_COMPUTE_SHADER, "WaterSurfaceHeightCS.glsl" }
	};
	waterSurfaceHeight.programName = "WaterSurfaceHeight";
	shaderProgramInfos.push_back(waterSurfaceHeight);

	shaderProgramCreateInfo waterSurfaceIFFT1;
	waterSurfaceIFFT1.shaderInfos = {
		{ GL_COMPUTE_SHADER, "WaterSurfaceIFFT1CS.glsl" }
	};
	waterSurfaceIFFT1.programName = "WaterSurfaceIFFT1";
	shaderProgramInfos.push_back(waterSurfaceIFFT1);

	shaderProgramCreateInfo waterSurfaceIFFT2;
	waterSurfaceIFFT2.shaderInfos = {
		{ GL_COMPUTE_SHADER, "WaterSurfaceIFFT2CS.glsl" }
	};
	waterSurfaceIFFT2.programName = "WaterSurfaceIFFT2";
	shaderProgramInfos.push_back(waterSurfaceIFFT2);

	// Vertex
	std::vector<GLfloat> vertices = std::vector<GLfloat>{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	// Texture
	/*int width, height, channels;
	unsigned char *ht_map = SOIL_load_image
	(
	"Ph256x6rnd.jpg",
	&width, &height, &channels,
	SOIL_LOAD_RGBA
	);*/

	glGenTextures(1, &h0Tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, h0Tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texSize, texSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, h0Tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glGenTextures(1, &bfTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, bfTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texSize, texSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(2, bfTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	ifftTexs[0].id = 1;
	ifftTexs[1].id = 3;
	for (int i = 0; i < 2; i++) {
		glGenTextures(1, &ifftTexs[i].ptr);
		glActiveTexture(GL_TEXTURE0 + ifftTexs[i].id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, ifftTexs[i].ptr);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, texSize, texSize, ifftCount);
		//glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, texSize, texSize, 3, 0, GL_RGBA, GL_FLOAT,
		//	NULL);
		glBindImageTexture(ifftTexs[i].id, ifftTexs[i].ptr, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	glGenTextures(1, &dTex);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, dTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texSize, texSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(4, dTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glGenTextures(1, &nTex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, nTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, texSize, texSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(5, nTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// Buffer
	revIndices = reverseBitIndices(texSize);
	glGenBuffers(1, &indicesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, revIndices.size() * sizeof(int), revIndices.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, indicesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


Water::~Water() { }

void Water::draw() {
	
	if (!isDrawInit) {
		// WaterSurfaceHeightInit
		glUseProgram(shaderPrograms["WaterSurfaceHeightInit"]);

		glDispatchCompute(256 / 16, 256 / 16, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		
		// WaterSurfaceButterfly
		glUseProgram(shaderPrograms["WaterSurfaceButterfly"]);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, indicesBuffer);
		glDispatchCompute(8, 256 / 16, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		
		isDrawInit = true;
	}

	// WaterSurfaceHeight
	glUseProgram(shaderPrograms["WaterSurfaceHeight"]);

	glUniform1f(glGetUniformLocation(shaderPrograms["WaterSurfaceHeight"], "t"), glfwGetTime());

	glDispatchCompute(256 / 16, 256 / 16, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	// WaterSurfaceIFFT1
	glUseProgram(shaderPrograms["WaterSurfaceIFFT1"]);

	glUniform1i(glGetUniformLocation(shaderPrograms["WaterSurfaceIFFT1"], "ifftCount"), ifftCount);

	int swap = 0;
	for (int direction = 0; direction < 2; direction++) {
		for (int stage = 0; stage < log2(texSize); stage++) {
			glUniform1i(glGetUniformLocation(shaderPrograms["WaterSurfaceIFFT1"], "direction"), direction);
			glUniform1i(glGetUniformLocation(shaderPrograms["WaterSurfaceIFFT1"], "stage"), stage);
			glUniform1i(glGetUniformLocation(shaderPrograms["WaterSurfaceIFFT1"], "swap"), swap);

			glDispatchCompute(256 / 16, 256 / 16, 1);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			swap = (swap + 1) % 2;
		}
	}

	// WaterSurfaceIFFT2
	glUseProgram(shaderPrograms["WaterSurfaceIFFT2"]);

	glUniform1i(glGetUniformLocation(shaderPrograms["WaterSurfaceIFFT2"], "ifftCount"), ifftCount);
	glUniform1i(glGetUniformLocation(shaderPrograms["WaterSurfaceIFFT2"], "swap"), swap);

	glDispatchCompute(256 / 16, 256 / 16, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	

	// WaterRendering
	glUseProgram(shaderPrograms["WaterRendering"]);

	glm::mat4 Model;
	Model = glm::translate(Model, glm::vec3(0.0f, -1.0f, -20.0f));
	Model = glm::rotate(Model, glm::radians(90.0f), glm::vec3(1, 0, 0));
	Model = glm::scale(Model, glm::vec3(100.0f));

	glm::mat4 model = Model;
	GLuint modelLoc = glGetUniformLocation(shaderPrograms["WaterRendering"], "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glUniform1i(glGetUniformLocation(shaderPrograms["WaterRendering"], "ourTexture"), 4);
	glUniform1i(glGetUniformLocation(shaderPrograms["WaterRendering"], "nTexture"), 5);
	glUniform1f(glGetUniformLocation(shaderPrograms["WaterRendering"], "t"), glfwGetTime());

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, 6);
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

std::vector<int> Water::reverseBitIndices(int size) {
	std::vector<int> indices;

	for (int i = 0; i < size; i++) {
		uint32_t revIndex = 0, index = i;
		for (int j = 0; j < log2(size); j++) {
			revIndex <<= 1;
			revIndex |= index & 1;
			index >>= 1;
		}
		indices.push_back(revIndex);
	}

	return indices;
}

