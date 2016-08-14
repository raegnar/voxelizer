//*****************************************************************************
//VoxelizerAsset.cpp***************************************************************
//*****************************************************************************

//Header Include
#include "VoxelizerAsset.h"

//Standard Library Includes
#include <sstream>


//*****************************************************************************
//Constructor******************************************************************
//*****************************************************************************
VoxelizerAsset::VoxelizerAsset() : AssetImpl()
{
	init();
}
//----------------------------------------------------------------------------------
VoxelizerAsset::VoxelizerAsset(const string &filename) : AssetImpl()
{
	this->load(filename);

	init();
}
//----------------------------------------------------------------------------------
void VoxelizerAsset::init()
{
	scale = 1.0f;


	VertFile assetMultirateVertFile = VertFile("./shaders/voxelizerAsset.vert");
	GeomFile assetMultirateGeomFile = GeomFile("./shaders/voxelizerAsset.geom");
	FragFile assetMultirateFragFile = FragFile("./shaders/voxelizerAsset.frag");


	VertFile assetWarpPerfectVertFile = VertFile("./shaders/warpPerfectAsset.vert");

	assetWarpTileShader.load(assetMultirateVertFile, assetMultirateGeomFile, assetMultirateFragFile);
}
//*****************************************************************************
//Destructor*******************************************************************
//*****************************************************************************
VoxelizerAsset::~VoxelizerAsset()
{
}
//*****************************************************************************
//Load*************************************************************************
//*****************************************************************************
void VoxelizerAsset::load(const string &filename)
{
	AssetImpl::load(filename);
	this->normalize();
}
//*****************************************************************************
//Update***********************************************************************
//*****************************************************************************
void VoxelizerAsset::update()
{
	mat4f scaleMatrix;
	scaleMatrix.scale_by(vec3f(scale,scale,scale));

	modelMatrix =  scaleMatrix * q.matrix() * transform;
}

void VoxelizerAsset::render(const mat4f &projMatrix,
							const mat4f &viewMatrix,
							const mat4f &modelMatrix)
{
	assetWarpTileShader.begin();
	assetWarpTileShader.uniform("projMatrix",  projMatrix);
	assetWarpTileShader.uniform("viewMatrix",  viewMatrix);
	assetWarpTileShader.uniform("modelMatrix", transform);
	assetWarpTileShader.uniform("warpMatrix",  mat4f());
	{
		for(uint i = 0; i < meshes.size(); ++i)
		{
			const Mesh &mesh = *meshes[i];

			assetWarpTileShader.uniformBlock("Material", mesh.material_buffer);

			if(mesh.diffuseTex)	assetWarpTileShader.uniformSampler("texUnit", mesh.diffuseTex, 0);

			mesh.buffer_object->renderElementsTriangles();
		}
	}
	assetWarpTileShader.end();

}
