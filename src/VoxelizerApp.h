
#pragma once

//Standard Library Includes
#include <iostream>
#include <vector>

//Render Library Includes
#include <render/shader.h>
#include <render/app.h>
#include <render/gl.h>
#include <render/asset.h>
#include <render/overlay.h>
#include <render/capture.h>


#include "VoxelBase.h"

// Predeclarations
class VoxelAsset;

//Namespaces
using namespace std;
using namespace render;
using namespace render::capture;

typedef VoxelBase VoxelStore;

class VoxelizerApp : public App
{
private:

	typedef texture::Texture2D<GLubyte, texture::rgba8,    texture::rgba >   tex2f_msaa;
	typedef texture::Texture2D<GLuint,  texture::r32ui,    texture::red_int> idTex_msaa;
	typedef texture::Texture2D<float,   texture::depth32f, texture::depth>   DepthTex_msaa;

	typedef texture::Texture2D<float,   texture::rgba32f,  texture::rgba > tex2f;
	typedef texture::Texture2D<float,   texture::depth32f, texture::depth> DepthTex;
	typedef texture::Texture2D<GLubyte, texture::rgb,      texture::rgb  > tex2ub;

	tex2f_msaa    *screenTex;	// screen texture
	idTex_msaa    *idTex;
	DepthTex_msaa *depthTex;

	tex2ub *screenDisplayTex;

	string selectedModel;
	string modelDir;


	ShaderProgram defaultShader;

	ShaderProgram depthVisShader;
	ShaderProgram idVisShader;

	ShaderProgram screenOverlayShader;

	TextureCapture frame;

	GLuint screenVAO;


	void renderIMGUI();


    enum VoxelizationMethod
    {
        fragVoxelize   = 0,
        triVoxelize    = 1,
        hybridVoxelize = 2,
    };

    VoxelizationMethod voxelizationMethod;

    enum VizMethod
    {
        defaultRender = 0,
        cubeCase      = 1,
        sphereCast    = 2,
    };




	VoxelAsset *asset  = NULL;
	VoxelStore *voxels = NULL;

	ScreenOverlay *screenOverlay;

	//Camera
	gl::Camera camera;

	mat4f transform;

	int   pow = 7;


	bool  slice;
	float sliceLevel;
	float sliceLayer;
	bool  cubeCast;
	bool  renderVoxels;
	bool  clearVoxels;
	bool  showTimingData;
	bool  showDiffuse;
	bool  showVoxPos;
	bool  showSpecular;
	bool  showBumpNormal;
	bool  showSurfNormal;
	bool  showDepth = false;

	bool fixedCam;

	bool showRSMdirectLight;
	bool showRSMposition;
	bool showRSMnormal;
	bool showRSMdepth;


	bool hideUI;
	bool showSceneUI;

//	CheckBox *timingData;

	//VoxelMethods
	bool persistentVoxelize;
















public:

	//Constructor
	VoxelizerApp(string, int, int, const std::vector<string> &);

	//Implementations
	void init();
	void initCam();
	void render();
	void update();

	void mousemove();

	void resize();


	mat4f modelMat;
};


