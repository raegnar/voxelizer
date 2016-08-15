//*****************************************************************************
//IsoVoxels.h******************************************************************
//*****************************************************************************

#pragma once


//Parent Include
#include "VoxelBase.h"

//Namespaces
using namespace std;
using namespace render;
using namespace gl;


class IsoVoxels : public VoxelBase
{
private:
//	typedef texture::Texture3D<float,   texture::rgba32f, texture::rgba> IsoTex;
	typedef texture::Texture3D<GLubyte, texture::rgba8,   texture::rgba> IsoTex;

public:
	IsoTex voxels;


public:

	IsoVoxels(int pow, int screenW, int screenH);

	void init(int pow, int screenW, int screenH);

	virtual string name() { return "IsoVoxels"; }

	////////
	//Cast//
	////////
	string getCastIncludes()	{ return "isoCubeSphereHit.glsl"; }
	void   setCastUniforms(ShaderProgram &shader, TexUnit texUnit, int mipLevel)
	{
		if(bindlessTextures)
			shader.uniformTextureHandle("Voxels", &voxels);
		else
			shader.uniform("Voxels", &voxels, texUnit);
	}

	/////////
	//Store//
	/////////
	string getStoreIncludes()	{ return "isoVoxelStore.glsl"; }
	void   setStoreUniforms(ShaderProgram &shader, ImageUnit imgUnit)
	{
//		if(bindlessTextures)
//			shader.uniformImageHandle("Voxels", &voxels, MipLevel(0));
//		else
			shader.uniform("Voxels", &voxels, imgUnit, MipLevel(0));
	}

	/////////
	//Fetch//
	/////////
	string getFetchIncludes()	{ return "isoVoxelFetch.glsl"; }
	void   setFetchUniforms(ShaderProgram &shader, TexUnit texUnit)
	{
		shader.uniformTextureHandle("Voxels", &voxels);
	}

	/////////
	//Clear//
	/////////
	string getClearIncludes()	{ return "isoClear.glsl"; }
	void   setClearUniforms(ShaderProgram &shader, int mipLevel)
	{
//		shader.uniformImageHandle("Voxels", &voxels,   mipLevel);
//		shader.uniformImageHandle("Mutex",  &mutexTex, mipLevel);
	}

	void sparseClear();

	void renderVoxels(const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel = 0);

	void renderSlice(const int sliceNum, const int mipLevel, const vec2f &bl, const vec2f &tr);

	void clearTextures()
	{
		voxels.clearAllLevels();
	}



};