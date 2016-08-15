//*****************************************************************************
//vboCube.cpp******************************************************************
//*****************************************************************************

#include "vboCube.h"

#include <render/gl.h>

VBOCube::VBOCube(float sideLen) : BufferObject()
{
	float side	= sideLen;
	float side2 = side / 2.0f;

	float v[24*3] =
	{
		// Front
	   -side2, -side2, side2,
		side2, -side2, side2,
		side2,  side2, side2,
	   -side2,  side2, side2,
	   // Right
		side2, -side2, side2,
		side2, -side2, -side2,
		side2,  side2, -side2,
		side2,  side2, side2,
	   // Back
	   -side2, -side2, -side2,
	   -side2,  side2, -side2,
		side2,  side2, -side2,
		side2, -side2, -side2,
	   // Left
	   -side2, -side2, side2,
	   -side2,  side2, side2,
	   -side2,  side2, -side2,
	   -side2, -side2, -side2,
	   // Bottom
	   -side2, -side2, side2,
	   -side2, -side2, -side2,
		side2, -side2, -side2,
		side2, -side2, side2,
	   // Top
	   -side2,  side2, side2,
		side2,  side2, side2,
		side2,  side2, -side2,
	   -side2,  side2, -side2
	};

	float n[24*3] =
	{
		// Front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		// Right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		// Back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		// Left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		// Bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		// Top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	float t[24*2] =
	{
		// Front
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Right
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Back
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Left
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Bottom
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// Top
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	GLuint el[] =
	{
		 0, 1, 2,
		 0, 2, 3,
		 4, 5, 6,
		 4, 6, 7,
		 8, 9,10,
		 8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23
	};

/*
	GLuint el[] =
	{ 
		 0, 1, 2,
		 0, 3, 2,
		 4, 5, 6,
		 4, 7, 6,
		 8, 9,10,
		 8,11,10,
		12,13,14,
		12,15,14,
		16,17,18,
		16,19,18,
		20,21,22,
		20,23,22
	};
*/
	vertexBuffer.setData (24, (vec3f  *)&v[0]);
	normalBuffer.setData (24, (vec3f  *)&n[0]);
	textureBuffer.setData(24, (vec2f  *)&t[0]);
	indexBuffer.setData  (36, (GLuint *)&el[0]);

	this->addBuffer(&vertexBuffer);
	this->addBuffer(&normalBuffer);
	this->addBuffer(&textureBuffer);
	this->addBuffer(&indexBuffer);
}
