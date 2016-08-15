//*****************************************************************************
//VoxelBase.cpp****************************************************************
//*****************************************************************************

//Header Include
#include "VoxelBase.h"

//Initialize Static Variables (all the stuff touched by the GUI)

bool  VoxelBase::sparseVoxelBuffer  = false;
float VoxelBase::cutoff             = 2.0f;

bool VoxelBase::bindlessTextures    = true;

//Atomic Max and Atomic Avg flags
bool VoxelBase::isoAtomicMaxOn   = true;
bool VoxelBase::isoAtomicAvgOn   = false;
bool VoxelBase::anisoAtomicMaxOn = true;
bool VoxelBase::anisoAtomicAvgOn = false;

//*****************************************************************************
//Constructor******************************************************************
//*****************************************************************************
VoxelBase::VoxelBase( int pow, int screenW, int screenH )
{
	this->init(pow, screenW, screenH);
	
//	overlay = new Overlay();

//	defaultShader.load(VertFile("./shaders/DefaultRender.vert"), FragFile("./shaders/DefaultRender.frag"));
	defaultShader.load(VertFile("./shaders/DefaultRender.vert"), GeomFile("./shaders/DefaultRender.geom"), FragFile("./shaders/DefaultRender.frag"));

	diffuseTex    = new tex2f(screenW, screenH);
	specularTex   = new tex2f(screenW, screenH);
	transmissive  = new tex2f(screenW, screenH);
	positionTex   = new tex2f(screenW, screenH);
	bumpNormalTex = new tex2f(screenW, screenH);
	surfNormalTex = new tex2f(screenW, screenH);

	mutexTex.init(w, h, d, numLevels);
	mutexTex.set_min_filter(GL_NEAREST);
	mutexTex.set_mag_filter(GL_NEAREST);
	if(glewIsSupported("GL_ARB_bindless_texture"))
		mutexTex.initHandles();

	activeVoxelIndirectElementDefaultCommand.count 				= 36;
	activeVoxelIndirectElementDefaultCommand.primCount 			= 0;
	activeVoxelIndirectElementDefaultCommand.firstIndex			= 0;
	activeVoxelIndirectElementDefaultCommand.baseVertex			= 0;
	activeVoxelIndirectElementDefaultCommand.reservedMustBeZero	= 0;

	activeVoxelIndirectArrayDefaultCommand.count 				= 0;
	activeVoxelIndirectArrayDefaultCommand.primCount 			= 1;
	activeVoxelIndirectArrayDefaultCommand.first				= 0;
	activeVoxelIndirectArrayDefaultCommand.reservedMustBeZero	= 0;


//	for(int level = 0; level < numLevels; level++)
	{
		int w = dim.x;// >> level;
		int h = dim.y;// >> level;
		int d = dim.z;// >> level;

		int denom = max(8/* >> level*/, 1);

		int elements = 3*w*h*d/denom;

		activeVoxelBuffer = new VoxelPosBufferTex(elements, NULL/*, UsageDynamicCopy()*/);

		activeVoxelIndirectArrayCommandBuffer = new IndirectArrayBufferTex(1, &activeVoxelIndirectArrayDefaultCommand/*, UsageDynamicCopy()*/);

		activeVoxelObject = new BufferObject();
		activeVoxelObject->addBuffer(activeVoxelBuffer);
	}

	instancedCubes = new VBOCube(1.0f);

#if USE_DEPTH_STENCIL == YES
	depthStencilTex = new DepthStencilTex(screenW, screenH);
#else
	depthTex = new DepthTex(screenW, screenH);
//	depthTex->parameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//	depthTex->parameter(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
#endif

	deferredFrameBuffer.attach(diffuseTex);
	deferredFrameBuffer.attach(specularTex);
	deferredFrameBuffer.attach(positionTex);
	deferredFrameBuffer.attach(bumpNormalTex);
	deferredFrameBuffer.attach(surfNormalTex);
#if USE_DEPTH_STENCIL == YES
	deferredFrameBuffer.attachDepthStencil(depthStencilTex);
#else
	deferredFrameBuffer.attachDepth(depthTex);
#endif

	glGenVertexArrays(1, &someVAO);	//VAO makes graphics debugger happy



	largeTriCounter = new AtomicCounterBuffer(1, NULL, UsageDynamicDraw());
	voxelCounter    = new AtomicCounterBuffer(1, NULL, UsageDynamicDraw());


}
//-----------------------------------------------------------------------------
void VoxelBase::init( int pow, int screenW, int screenH )
{
	numLevels = pow+1;
	w = h = d = (int)std::pow(2.0f, pow);
	dim = vec3i(w,h,d);
	worldToVoxel = mat4f((float)w,        0,        0, w*.5f,
	                            0, (float)h,        0, h*.5f,
	                            0,        0, (float)d, d*.5f,
	                            0,        0,        0,  1 );

	float l = 0.0f;	//left
	float b = 0.0f;	//bottom
	float n = 0.0f;	//near

	float r = (float)w;	//right
	float t = (float)h;	//top
	float f = (float)d;	//far

	float inv_dx = 1.0f / (r-l);
	float inv_dy = 1.0f / (t-b);
	float inv_dz = 1.0f / (f-n);

	orthoMatrix = mat4f( 2*inv_dx,        0,               0,              0,
	                     0,               2*inv_dy,        0,              0,
	                     0,               0,               2*inv_dz,       0,
	                    -1*(r+l)*inv_dx, -1*(t+b)*inv_dy, -1*(f+n)*inv_dz, 1);
}
//*****************************************************************************
//InitShaders******************************************************************
//*****************************************************************************
void VoxelBase::initShaders()
{
	//Dense Clear
	denseClearVertFile     = VertFile("./shaders/denseClear.vert");
	denseClearFragFile     = FragFile("./shaders/denseClear.frag");
	denseClearShader.load(denseClearVertFile, denseClearFragFile.include(this->getClearIncludes()));

	sparseClearVertFile    = VertFile("./shaders/sparseClear.vert");

	//Casting Shaders
	castVertFile           = VertFile("./shaders/cast.vert");

	//CubeCast Shader
	cubeCastFragFile       = FragFile("./shaders/cubeCast.frag").include(this->getCastIncludes());
	cubeCastShader.load(castVertFile, cubeCastFragFile);

	//SphereCast Shader
	sphereCastFragFile     = FragFile("./shaders/sphereCast.frag").include(this->getCastIncludes());
	sphereCastShader.load(castVertFile, sphereCastFragFile);

	//Voxelize Hybrid
	voxelizeHybridVertFile = VertFile("./shaders/voxelizeHybrid.vert");
	voxelizeHybridGeomFile = GeomFile("./shaders/voxelizeHybrid.geom").include(this->getStoreIncludes());
	voxelizeHybridShader.load(voxelizeHybridVertFile, voxelizeHybridGeomFile);

	//Voxelize
	voxelizeFragVertFile   = VertFile("./shaders/voxelizeFrag.vert");
	voxelizeFragGeomFile   = GeomFile("./shaders/voxelizeFrag.geom");
	voxelizeFragFragFile   = FragFile("./shaders/voxelizeFrag.frag").include(this->getStoreIncludes());
	voxelizeFragShader.load(voxelizeFragVertFile, voxelizeFragGeomFile, voxelizeFragFragFile);

	renderVoxelsVertFile   = VertFile("./shaders/renderVoxels.vert");
	renderVoxelsFragFile   = FragFile("./shaders/renderVoxels.frag");

}
//*****************************************************************************
//Update***********************************************************************
//*****************************************************************************
void VoxelBase::update(int screenW, int screenH)
{
	if(diffuseTex->w != screenW || diffuseTex->h != screenH)
	{
		diffuseTex->resize(screenW, screenH);
		specularTex->resize(screenW, screenH);
		transmissive->resize(screenW, screenH);
		positionTex->resize(screenW, screenH);
		bumpNormalTex->resize(screenW, screenH);
		surfNormalTex->resize(screenW, screenH);
	//	depthStencilTex->resize(screenW, screenH);
		depthTex->resize(screenW, screenH);
	}

	glClearColor(.5,.5,.5,0);
	deferredFrameBuffer.clear();

//	profiler.update();
}
//*****************************************************************************
//Voxelize*********************************************************************
//*****************************************************************************
void VoxelBase::voxelize( VoxelAsset *asset, const gl::Camera &camera )
{
	if(VoxelBase::sparseVoxelBuffer)
	{
		sparseVoxelize(asset, camera);
	}
	else
	{
		denseVoxelize(asset, camera);
	}
	this->postVoxelize();
}
//*****************************************************************************
//Clear************************************************************************
//*****************************************************************************
void VoxelBase::clear()
{
//	if(VoxelBase::voxelBuffer)
//	{
//		sparseClear();
//	}
//	else
//	{
		denseClear();
//	}

	//Calling this for both, should only really be necessary for sparse
	VoxelBase::clearCommands();
}
//*****************************************************************************
//Render***********************************************************************
//*****************************************************************************
void VoxelBase::renderToTextures(const gl::Camera &camera, VoxelAsset *asset)
{
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	deferredFrameBuffer.begin();
	{
		defaultShader.begin();
	//	defaultShader.uniformBlock(  "Matrices",      asset->matrixBuffer);
		defaultShader.uniform("cameraPos",    camera._pos);
		defaultShader.uniform("projMatrix",   camera.proj_mat);
		defaultShader.uniform("viewMatrix",   camera.view_mat);
		defaultShader.uniform("modelMatrix",  asset->modelMatrix);
		{
			for(size_t i = 0; i < asset->meshes.size(); ++i)
			{
				const Mesh &mesh = *(asset->meshes[i]);

				if(mesh.normalTex)		defaultShader.uniformSampler("normalTex",   mesh.normalTex,   TexUnit(0));
				if(mesh.specularTex)	defaultShader.uniformSampler("specularTex", mesh.specularTex, TexUnit(1));
				if(mesh.diffuseTex)		defaultShader.uniformSampler("diffuseTex",  mesh.diffuseTex,  TexUnit(2));

//				GLuint bonesLocation = glGetUniformLocation(defaultShader.id(), "bones");
//				glUniformMatrix4fv(bonesLocation, (GLsizei)mesh.boneMatrices.size(), GL_FALSE, mesh.absoluteTransforms);
//				defaultShader.uniform(       "numBones",   (GLuint)mesh.boneMatrices.size());
				defaultShader.uniformBlock(  "Material",   mesh.material_buffer);
				{
					mesh.buffer_object->renderElementsTrianglesUnclosed();
				}
			}
			BufferObject::unbind();
		}
		defaultShader.end();
	}
	deferredFrameBuffer.end();
}


void VoxelBase::enableDisableVoxelBuffer( bool enableDisable )
{
	VoxelBase::sparseVoxelBuffer = enableDisable;
	voxelizeFragShader.setDefine("VOXEL_BUFFER", enableDisable ? "ENABLED" : "DISABLED");
//	cerr << voxelizeFragShader.frag_shader->source << endl;
}

void VoxelBase::clearCommands()
{
	GLuint zeroUint = 0;
#ifdef OPENGL_4_3
//	glClearNamedBufferDataEXT(activeVoxCounter.id(), GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zeroUint);
#else

#endif
//	activeVoxIndirectElementCommandBuffers[0]->setData(1, &activeVoxIndirectElementDefaultCommand);

	glClearNamedBufferDataEXT(voxelCounter->id(), GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zeroUint);

	activeVoxelIndirectArrayCommandBuffer->setData(1, &activeVoxelIndirectArrayDefaultCommand);
}

//*****************************************************************************
//Ray Casting Debug Visualization**********************************************
//*****************************************************************************
void VoxelBase::cubeCast( const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel /*= 0*/ )
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	vec3i volumeDim;
	volumeDim.x = dim.x >> mipLevel;
	volumeDim.y = dim.y >> mipLevel;
	volumeDim.z = dim.z >> mipLevel;

	float w = (float)volumeDim.x;
	float h = (float)volumeDim.y;
	float d = (float)volumeDim.z;

	mat4f unitToVoxel = mat4f(w, 0, 0, w*.5f,
	                          0, h, 0, h*.5f,
	                          0, 0, d, d*.5f,
	                          0, 0, 0, 1   );

	mat4f unitToVoxelTrans = transpose(unitToVoxel);

	vec3f rayOrigin = camera._pos * unitToVoxelTrans;
	vec3f lightPos  = vec3f(2,2,2) * unitToVoxelTrans;

	cubeCastShader.begin();
	cubeCastShader.uniform("mipLevel",    mipLevel);
	cubeCastShader.uniform("projMatrix",  camera.proj_mat);
	cubeCastShader.uniform("viewMatrix",  camera.view_mat);
	cubeCastShader.uniform("modelMatrix", mat4f());
	cubeCastShader.uniform("unitToVoxel", unitToVoxelTrans);
	cubeCastShader.uniform("rayOrigin",   rayOrigin);
	cubeCastShader.uniform("volumeDim",   volumeDim);
	this->setCastUniforms(cubeCastShader, TexUnit(0), mipLevel);
	{
		cube.renderElementsTriangles();
	}
	cubeCastShader.end();
}
//-----------------------------------------------------------------------------
void VoxelBase::sphereCast( const gl::Camera &camera, const mat4f &modelMatrix, int mipLevel /*= 0*/ )
{
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	vec3i volumeDim;
	volumeDim.x = dim.x >> mipLevel;
	volumeDim.y = dim.y >> mipLevel;
	volumeDim.z = dim.z >> mipLevel;

	float w = (float)volumeDim.x;
	float h = (float)volumeDim.y;
	float d = (float)volumeDim.z;

	mat4f unitToVoxel = mat4f(w, 0, 0, w*.5f,
	                          0, h, 0, h*.5f,
	                          0, 0, d, d*.5f,
	                          0, 0, 0, 1   );
	
	mat4f unitToVoxelTrans = transpose(unitToVoxel);

	vec3f rayOrigin = camera._pos * unitToVoxelTrans;
	vec3f lightPos  = vec3f(2,2,2) * unitToVoxelTrans;

	sphereCastShader.begin();
	sphereCastShader.uniform("mipLevel",    mipLevel);
	sphereCastShader.uniform("projMatrix",  camera.proj_mat);
	sphereCastShader.uniform("viewMatrix",  camera.view_mat);
	sphereCastShader.uniform("modelMatrix", mat4f());
	sphereCastShader.uniform("unitToVoxel", unitToVoxelTrans);
	sphereCastShader.uniform("rayOrigin",   rayOrigin);
	sphereCastShader.uniform("volumeDim",   volumeDim);
	this->setCastUniforms(sphereCastShader, TexUnit(0), mipLevel);
	{
		cube.renderElementsTriangles();
	}
	sphereCastShader.end();
}

//*****************************************************************************
//Clear************************************************************************
//*****************************************************************************
void VoxelBase::denseClear()
{
	if(sparseVoxelBuffer == true)
	{
		mutexTex.clearAllLevels();
	}

	this->clearTextures();
}

//*****************************************************************************
//Voxelize*********************************************************************
//*****************************************************************************
void VoxelBase::denseVoxelize( VoxelAsset *asset, const gl::Camera &camera )
{
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, w, h);
	voxelizeFragShader.begin();
	voxelizeFragShader.uniform("volumeDim",       dim);
	voxelizeFragShader.uniform("modelMatrix",     asset->modelMatrix);
	voxelizeFragShader.uniform("orthoMatrix",     orthoMatrix);
	voxelizeFragShader.uniform("unitToVoxel",     worldToVoxel);
	this->setStoreUniforms(voxelizeFragShader, ImageUnit(0));
//	voxelCounter->bindBase(1);
	{
		for(size_t i = 0; i < asset->meshes.size(); ++i)
		{
			const Mesh &mesh = *(asset->meshes[i]);
			
//			GLuint bonesLocation = glGetUniformLocation(voxelizeFragShader.id(), "bones");
//			glUniformMatrix4fv(bonesLocation, (GLsizei)mesh.boneMatrices.size(), GL_FALSE, mesh.absoluteTransforms);
//			voxelizeFragShader.uniform(       "numBones",      (GLuint)mesh.boneMatrices.size());
			voxelizeFragShader.uniformBlock(  "Material",      mesh.material_buffer);
			voxelizeFragShader.uniform(       "meshTransform", mesh.transform);
			if(mesh.diffuseTex)	voxelizeFragShader.uniformSampler("diffuseTex", mesh.diffuseTex, TexUnit(0));
			{
				mesh.buffer_object->renderElementsTrianglesUnclosed();
			}
		}
		BufferObject::unbind();
	}
	voxelizeFragShader.end();

	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

//	glMemoryBarrier(GL_ALL_BARRIER_BITS);
//	cerr << voxelCounter->getData()[0] << endl;
}
//-----------------------------------------------------------------------------
//Sparse Voxelize--------------------------------------------------------------
//-----------------------------------------------------------------------------
void VoxelBase::sparseVoxelize( VoxelAsset *asset, const gl::Camera &camera )
{
	int location;

	glDisable(GL_CULL_FACE);

//	GLuint zero = 0;
//	glClearTexImage(mutexTex.id(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &zero);

	mutexTex.clear();

	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glViewport(0, 0, w, h);
	voxelizeFragShader.begin();
	voxelizeFragShader.uniform("volumeDim",   dim);
	voxelizeFragShader.uniform("modelMatrix", asset->modelMatrix);
	voxelizeFragShader.uniform("orthoMatrix", orthoMatrix);
	voxelizeFragShader.uniform("unitToVoxel", worldToVoxel);
	//This binds the count element of the activeVoxIndirectArrayCommandBuffers directly as an atomic counter in the shader (no need for copy from dedicated atomic counter)
	glBindBufferRange(                    GL_ATOMIC_COUNTER_BUFFER,	//Target
	                                                             0,	//index (i.e. binding point in shader)
	                  activeVoxelIndirectArrayCommandBuffer->id(),	//buffer name
	                                                             0,	//offset
	                                               sizeof(GLuint));	//size

	this->setStoreUniforms(voxelizeFragShader, ImageUnit(0));

//	if(bindlessTextures)
//		voxelizeFragShader.uniformImageHandle("VoxelMutex", &mutexTex, MipLevel(0));
//	else
		voxelizeFragShader.uniform("VoxelMutex", &mutexTex, ImageUnit(1));

	if(bindlessTextures)
	{
		location = voxelizeFragShader.getUniformLoc("VoxelBuffer");
		glProgramUniformHandleui64ARB(voxelizeFragShader.id(), location, activeVoxelBuffer->imgHandle);
	}
	else
		voxelizeFragShader.uniformImage("VoxelBuffer", mutexTex.id(), ImageUnit(2), GL_R32UI);


	{
		for(size_t i = 0; i < asset->meshes.size(); ++i)
		{
			const Mesh &mesh = *(asset->meshes[i]);
//			GLuint bonesLocation = glGetUniformLocation(voxelizeFragShader.id(), "bones");
//			glUniformMatrix4fv(bonesLocation, (GLsizei)mesh.boneMatrices.size(), GL_FALSE, mesh.absoluteTransforms);
//			voxelizeFragShader.uniform(       "numBones",   (GLuint)mesh.boneMatrices.size());
			voxelizeFragShader.uniformBlock(  "Material",   mesh.material_buffer);
			if(mesh.diffuseTex)	voxelizeFragShader.uniformSampler("diffuseTex", mesh.diffuseTex, TexUnit(0));
			{
				mesh.buffer_object->renderElementsTrianglesUnclosed();
			}
		}
		BufferObject::unbind();
	}

}
//-----------------------------------------------------------------------------
//Hybrid Voxelize--------------------------------------------------------------
//-----------------------------------------------------------------------------
void VoxelBase::hybridVoxelize( VoxelAsset *asset, const gl::Camera &camera )
{
	GLuint zero = 0;

	glDisable(GL_CULL_FACE);

	glViewport(0, 0, w, h);

	voxelizeHybridShader.begin();
	voxelizeHybridShader.uniform("volumeDim",   dim);
	voxelizeHybridShader.uniform("modelMatrix", asset->modelMatrix);
	voxelizeHybridShader.uniform("unitToVoxel", worldToVoxel);
	voxelizeHybridShader.uniform("cutoff",      cutoff);
	this->setStoreUniforms(voxelizeHybridShader, ImageUnit(0));
	{
		for(size_t i = 0; i < asset->meshes.size(); ++i)
		{
			const Mesh &mesh = *(asset->meshes[i]);


			GLuint zeroUint = 0;
			glClearNamedBufferDataEXT(largeTriCounter->id(), GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, &zeroUint);

//			largeTriCounter->setData(1, &zero, UsageDynamicDraw());
			largeTriCounter->bindBase(1);

//			//This binds the count element of the activeVoxIndirectArrayCommandBuffers directly as an atomic counter in the shader (no need for copy from dedicated atomic counter)
//			glBindBufferRange(                    GL_ATOMIC_COUNTER_BUFFER,	//Target
//			                                                             1,	//index (i.e. binding point in shader)
//			               mesh.largeTriIndirectElementCommandBuffer->id(),	//buffer name
//			                                                             0,	//offset
//			                                               sizeof(GLuint));	//size

		//	mesh.largeTriIndirectElementCommandBuffer->getData()->print();

//			voxelizeHybridShader.uniformImage("irrelevant", mesh.largeTriIndexBuffer->texID(),                  ImageUnit(4), GL_R32UI);
//			voxelizeHybridShader.uniformImage("irrelevant", mesh.largeTriIndirectElementCommandBuffer->texID(), ImageUnit(5), GL_R32UI);
			mesh.buffer_object->renderElementsTriangles();
		}
	}
	voxelizeHybridShader.end();


	voxelizeFragShader.begin();
	voxelizeFragShader.uniform("volumeDim",   dim);
	voxelizeFragShader.uniform("modelMatrix", asset->modelMatrix);
	voxelizeFragShader.uniform("orthoMatrix", orthoMatrix);
	voxelizeFragShader.uniform("unitToVoxel", worldToVoxel);
	this->setStoreUniforms(voxelizeFragShader, ImageUnit(0));
	{
		for(size_t i = 0; i < asset->meshes.size(); ++i)
		{
			const Mesh &mesh = *(asset->meshes[i]);

//			GLuint bonesLocation = glGetUniformLocation(voxelizeFragShader.id(), "bones");
//			glUniformMatrix4fv(bonesLocation, (GLsizei)mesh.boneMatrices.size(), GL_FALSE, mesh.absoluteTransforms);
//			voxelizeFragShader.uniform(       "numBones",   (GLuint)mesh.boneMatrices.size());
			voxelizeFragShader.uniformBlock(  "Material",   mesh.material_buffer);
			if(mesh.diffuseTex)	voxelizeFragShader.uniformSampler("diffuseTex", mesh.diffuseTex, TexUnit(0));
			{
			//	cerr << "Mesh[" << i << "] count: " << mesh.largeTriIndirectElementCommandBuffer->getData()->count << endl;
			//	cerr << mesh.largeTriIndexBuffer->getData()[0] << endl;

//			mesh.largeTriIndirectElementCommandBuffer->bind();
//				{
//					mesh.largeTriBufferObject->renderElementsIndirectUnclosed(GL_TRIANGLES);
//				}
			//	mesh.largeTriBufferObject->renderElementsTrianglesUnclosed();
			////	mesh.largeTriBufferObject->renderElementsTriangles();
			}
		}
		BufferObject::unbind();
	}
	voxelizeFragShader.end();
	
}

void VoxelBase::persistentVoxelize( VoxelAsset *asset, const gl::Camera &camera )
{

}










