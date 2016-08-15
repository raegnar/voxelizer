//*****************************************************************************
//vboCube.h********************************************************************
//*****************************************************************************

#pragma once

#include <render/buffer.h>

using namespace render;
using namespace buffer;


class VBOCube : public BufferObject
{
private:
	typedef ArrayBufferT<vec3f>    VertexBuffer;
	typedef ArrayBufferT<vec3f>    NormalBuffer;
	typedef ArrayBufferT<vec2f>    TextureBuffer;

	typedef ElementBufferT<GLuint> IndexBuffer;

private:

	VertexBuffer  vertexBuffer;
	NormalBuffer  normalBuffer;
	TextureBuffer textureBuffer;

	IndexBuffer   indexBuffer;

public:
	VBOCube(float sideLen = 1.0f);
};

