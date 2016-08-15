//*****************************************************************************
//VoxelBase.h******************************************************************
//*****************************************************************************

#pragma once

//Render Library Includes
#include <render/shader.h>
#include <render/gl.h>
#include <render/asset.h>
#include <render/texture.h>
#include <render/buffer.h>
#include <render/capture.h>
#include <render/overlay.h>
#include <render/time.h>

//Standard Library Includes
#include <vector>
#include <unordered_map>

//Project Includes
#include "VoxelAsset.h"
#include "VBOCube.h"

//#include "BufferUtil.h"

//Namespaces
using namespace std;
using namespace render;
using namespace capture;
using namespace gl;

#define YES 1
#define NO  0

#define USE_DEPTH_STENCIL NO

class VoxelBase
{
protected:

	typedef texture::Texture3D<GLuint,            texture::r32ui,           texture::red_int     > MuTex;
	typedef texture::Texture3D<GLubyte,           texture::rgba8,           texture::rgba        > IsoTex;
	typedef texture::Texture3D<GLubyte,           texture::rgba8,           texture::rgba        > AlphaTex;
	typedef texture::Texture2D<float,             texture::depth32f,        texture::depth       > DepthTex;
//	typedef texture::Texture2D<GLubyte,           texture::depth,           texture::depth       > DepthTex;
//	typedef texture::Texture3D<GLubyte,           texture::rgba8,           texture::rgba        > NormalTex;
	typedef texture::Texture3D<float,             texture::rgba32f,         texture::rgba        > NormalTex;
	typedef texture::Texture2D<texture::uint24_8, texture::depth24stencil8, texture::depthStencil> DepthStencilTex;

	typedef ArrayBufferT<vec2f> PointBuffer;

	typedef AtomicCounterBufferT<GLuint> AtomicCounterBuffer;

	TextureCapture deferredFrameBuffer;
	
	typedef texture::Texture2D<float, texture::rgba32f, texture::rgba> tex2f;

public:

	////////////////////////
	//ScreenSpace Textures//
	////////////////////////
	union
	{
		struct 
		{
			tex2f *diffuseTex;
			tex2f *specularTex;
			tex2f *transmissive;
			tex2f *positionTex;
			tex2f *bumpNormalTex;
			tex2f *surfNormalTex;
		};
		tex2f *textures[6];
	};

public:

	//Universal Textures
	MuTex mutexTex;

	//Universal Buffers (and Counters)

	VoxelPosBufferTex *activeVoxelBuffer;

	DrawElementsIndirectCommand activeVoxelIndirectElementDefaultCommand;
	DrawArraysIndirectCommand   activeVoxelIndirectArrayDefaultCommand;

	IndirectArrayBufferTex *activeVoxelIndirectArrayCommandBuffer;
	BufferObject           *activeVoxelObject;

//	unordered_map<Mesh *, IndexBufferTex *> largeTriIndexBuffers;

#if USE_DEPTH_STENCIL == YES
	DepthStencilTex *depthStencilTex;
#else
	DepthTex *depthTex;
#endif

public:
	int pow;
	int w,h,d;
	vec3i dim;
	int numLevels;
public:


	static bool   isoAtomicMaxOn;
	static bool   isoAtomicAvgOn;
	static bool anisoAtomicMaxOn;
	static bool anisoAtomicAvgOn;

	static bool sparseVoxelBuffer;

	static float cutoff;

	static bool bindlessTextures;


protected:
	mat4f worldToVoxel;		//transforms and scales from (-0.5 to 0.5) to (0 to voxelDim)
	mat4f orthoMatrix;

	VBOCube cube;

	VBOCube *instancedCubes;

	AtomicCounterBuffer *largeTriCounter;
	AtomicCounterBuffer *voxelCounter;

public:
	Overlay overlay;

	ShaderProgram defaultShader;

	//Clear Shader
	VertFile      denseClearVertFile;
	FragFile      denseClearFragFile;
	ShaderProgram denseClearShader;

	//Voxelize Shader (Hybrid Method)
	VertFile      voxelizeHybridVertFile;
	GeomFile      voxelizeHybridGeomFile;
	ShaderProgram voxelizeHybridShader;

	//Voxelize Shader (Fragment Method)
	VertFile      voxelizeFragVertFile;
	GeomFile      voxelizeFragGeomFile;
	FragFile      voxelizeFragFragFile;
	ShaderProgram voxelizeFragShader;

	//Active Voxel Clear Shader
	ShaderFile    sparseClearVertFile;
	ShaderProgram sparseClearShader;

	//Casting Shaders
	VertFile castVertFile;

	//CubeCast Shader
	FragFile      cubeCastFragFile;
	ShaderProgram cubeCastShader;

	//SphereCast Shader
	FragFile      sphereCastFragFile;
	ShaderProgram sphereCastShader;

	//RenderVoxels shader (instanced rendering)
	VertFile      renderVoxelsVertFile;
	FragFile      renderVoxelsFragFile;
	ShaderProgram renderVoxelsShader;

	GLuint someVAO;	//VAO makes graphics debugger happy

public:

	VoxelBase(int pow, int screenW, int screenH);

	virtual void init(int pow, int screenW, int screenH);

	void initShaders();

	virtual ~VoxelBase() {cerr << "~VoxelBase";}

	virtual string name() { return "VoxelBase"; }

	
	void update(int screenW, int screenH);

	void renderToTextures(const gl::Camera &camera, VoxelAsset *asset);

	//Individual Effects
	virtual string getCastIncludes()													{ cerr << name() << "::getCastIncludes() not implemented!" << endl; return ""; }
	virtual void   setCastUniforms(ShaderProgram &shader, TexUnit texUnit, int mL)		{ cerr << name() << "::setCastUniforms() not implemented!" << endl; }

	virtual string getStoreIncludes()													{ cerr << name() << "::getStoreIncludes() not implemented!" << endl; return ""; }
	virtual void   setStoreUniforms(ShaderProgram &shader, ImageUnit imgUnit)			{ cerr << name() << "::setStoreUniforms() not implemented!" << endl; }

	virtual string getFetchIncludes()													{ cerr << name() << "::getFetchIncludes() not implemented!" << endl; return ""; }
	virtual void   setFetchUniforms(ShaderProgram &shader, TexUnit texUnit)				{ cerr << name() << "::setFetchUniforms() not implemented!" << endl; }

	virtual string getClearIncludes()													{ cerr << name() << "::getClearIncludes() not implemented!" << endl; return ""; }
	virtual void   setClearUniforms(ShaderProgram &shader, int mipLevel)				{ cerr << name() << "::setClearUniforms() not implemented!" << endl; }


	void           voxelize(VoxelAsset *asset, const gl::Camera &camera);
	void      denseVoxelize(VoxelAsset *asset, const gl::Camera &camera);
	void     sparseVoxelize(VoxelAsset *asset, const gl::Camera &camera);
	void     hybridVoxelize(VoxelAsset *asset, const gl::Camera &camera);

	void persistentVoxelize(VoxelAsset *asset, const gl::Camera &camera);

	virtual void postVoxelize() {}

	void       clear();
	void  denseClear();

	virtual void clearTextures()		{ cerr << name() << "::clearTextures() not implemented!" << endl; }
	virtual void sparseClear()			{ cerr << name() << "::sparseClear() not implemented!" << endl; }

	void clearCommands();
	

	void cubeCast(  const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel = 0);
	void sphereCast(const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel = 0);

	virtual void renderSlice(const int layer, const int mipLevel, const vec2f &bl, const vec2f &tr) {}

	virtual void renderVoxels(const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel = 0) {}

	const vec3i getDim() const { return dim; }


	void enableDisableVoxelBuffer(bool enableDisable);


};