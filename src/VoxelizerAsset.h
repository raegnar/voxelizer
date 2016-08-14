//*****************************************************************************
//VoxelizerAsset.h*****************************************************************
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

class VoxelizerAsset : public AssetImpl
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
	VoxelizerAsset();
	VoxelizerAsset(const string &);
	~VoxelizerAsset();

public:

	void load(const string &);

	void update();

	void render(const mat4f &projMatrix,
				const mat4f &viewMatrix,
				const mat4f &modelMatrix);



};