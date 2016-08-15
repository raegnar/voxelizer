//*****************************************************************************
//VoxelAsset.h*****************************************************************
//*****************************************************************************

#pragma once

//Render Library Includes
#include <render/shader.h>
#include <render/gl.h>
#include <render/asset.h>
#include <render/texture.h>
#include <render/buffer.h>

//Standard Library Includes
#include <vector>


//Namespaces
using namespace std;
using namespace render;
using namespace gl;


typedef AssetBufferObjects AssetImpl;

class VoxelAsset : public AssetImpl
{
public:
	quatf q;
	float scale;

public:

	mat4f modelMatrix;

	ShaderProgram assetWarpTileShader;
	ShaderProgram assetWarpPerfectShader;
private:
	void init();

public:

	//Constructor
	VoxelAsset();
	VoxelAsset(const string &);
	~VoxelAsset();

public:

	void load(const string &);

	void update();

	void render(const mat4f &projMatrix,
				const mat4f &viewMatrix,
				const mat4f &modelMatrix);



};