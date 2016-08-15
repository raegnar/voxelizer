//*****************************************************************************
//IsoVoxels.cpp****************************************************************
//*****************************************************************************

//Header Include
#include "IsoVoxels.h"

//Standard Library Include
#include <algorithm>


//*****************************************************************************
//Constructor******************************************************************
//*****************************************************************************
IsoVoxels::IsoVoxels(int pow, int screenW, int screenH) : VoxelBase(pow, screenW, screenH)
{
	this->initShaders();

	voxels.init(w, h, d, numLevels);
	voxels.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
	voxels.set_mag_filter(GL_LINEAR);
	if(glewIsSupported("GL_ARB_bindless_texture"))
		voxels.initHandles();

	renderVoxelsShader.load(renderVoxelsVertFile, renderVoxelsFragFile);

	//Initialize Textures
	this->denseClear(); 
}
//-----------------------------------------------------------------------------
void IsoVoxels::init(int pow, int screenW, int screenH)
{
	voxels.init(w, h, d, numLevels);
}
//*****************************************************************************
//Sparse Clear*****************************************************************
//*****************************************************************************
void IsoVoxels::sparseClear()
{
	glEnable(GL_RASTERIZER_DISCARD);
//	profiler.begin("Iso Active Clear");
	{
		sparseClearShader.begin();
		{
			sparseClearShader.uniformImage("irrelevant",   voxels.id(), 0, GL_RGBA8);
			sparseClearShader.uniformImage("irrelevant", mutexTex.id(), 2, GL_R32UI);

			activeVoxelIndirectArrayCommandBuffer->bind();
			{
				activeVoxelObject->renderArraysIndirect(GL_POINTS);
			}
			activeVoxelIndirectArrayCommandBuffer->unbind();

			glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
		}
		sparseClearShader.end();
	}
//	profiler.end();
	glDisable(GL_RASTERIZER_DISCARD);
}
//*****************************************************************************
//RenderVoxels*****************************************************************
//*****************************************************************************
void IsoVoxels::renderVoxels( const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel /*= 0*/ )
{
	DrawArraysIndirectCommand *activeVoxIndirectArrayCommand = activeVoxelIndirectArrayCommandBuffer->getData();
	activeVoxIndirectArrayCommand->print();

	vec3f gridRes;
	gridRes.x = (float)(w >> mipLevel);
	gridRes.y = (float)(h >> mipLevel);
	gridRes.z = (float)(d >> mipLevel);

	vec3f boxDim = 0.95f / gridRes;

	renderVoxelsShader.begin();
	renderVoxelsShader.uniform("projMatrix",         camera.proj_mat);
	renderVoxelsShader.uniform("viewMatrix",         camera.view_mat);
	renderVoxelsShader.uniform("modelMatrix",        mat4f());
	renderVoxelsShader.uniform("Light.Intensity",    vec3f(1.0f,1.0f,1.0f) );
	renderVoxelsShader.uniform("Light.Position",     vec4f(0.0f,0.0f,0.0f,1.0f) );
	renderVoxelsShader.uniform("Material.Kd",        vec3f(0.90f, 0.90f, 0.90f) );
	renderVoxelsShader.uniform("Material.Ks",        vec3f(0.95f, 0.95f, 0.95f) );
	renderVoxelsShader.uniform("Material.Ka",        vec3f(0.10f, 0.10f, 0.10f) );
	renderVoxelsShader.uniform("Material.Shininess", 100.0f);
	renderVoxelsShader.uniform("gridRes",            gridRes);
	renderVoxelsShader.uniform("boxDim",             boxDim);
	{
	//	activeVoxIndirectElementCommandBuffers[mipLevel]->bind();
	//	{
	//		instancedCubes[mipLevel]->renderElementsIndirect(GL_TRIANGLES);
	//	}
	//	activeVoxIndirectElementCommandBuffers[mipLevel]->unbind();

		instancedCubes->bind();
		{
			glDrawElementsInstanced(GL_TRIANGLES, 
			                        36,
			                        GL_UNSIGNED_INT,
			                        NULL,
			                        activeVoxIndirectArrayCommand->count);
		}
		instancedCubes->unbind();
	}
	renderVoxelsShader.end();


	delete activeVoxIndirectArrayCommand;
}
//*****************************************************************************
//Render Voxel Slice (for debugging)*******************************************
//*****************************************************************************
void IsoVoxels::renderSlice( const int sliceNum, const int mipLevel, const vec2f &bl, const vec2f &tr )
{
	overlay.renderMipmapLevelLayer(&voxels, sliceNum, mipLevel, bl, tr);
}

