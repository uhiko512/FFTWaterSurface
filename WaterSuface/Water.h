#pragma once

#include "RenderObject.h"

struct IFFTTex {
	GLuint ptr;
	int id;
};

class Water : public RenderObject {

private:
	bool isDrawInit;
	int texSize;
	int ifftCount;

	GLuint h0Tex;
	GLuint bfTex;
	GLuint indicesBuffer;
	GLuint dTex;
	GLuint nTex;
	IFFTTex ifftTexs[2];

	std::vector<int> revIndices;

public:
	Water();
	~Water();

	void draw() override;

private:
	std::vector<int> reverseBitIndices(int size);
};

