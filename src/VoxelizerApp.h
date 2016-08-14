
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

// Predeclarations
class VoxelizerAsset;

//Namespaces
using namespace std;
using namespace render;
using namespace render::capture;

class VoxelizerApp : public App
{
private:

	typedef texture::Texture2D<GLubyte, texture::rgba8,    texture::rgba >   tex2f_msaa;
	typedef texture::Texture2D<GLuint,  texture::r32ui,    texture::red_int> idTex_msaa;
	typedef texture::Texture2D<float,   texture::depth32f, texture::depth>   DepthTex_msaa;

	typedef texture::Texture2D<float,   texture::rgba32f,  texture::rgba > tex2f;
	typedef texture::Texture2D<float,   texture::depth32f, texture::depth> DepthTex;
	typedef texture::Texture2D<GLubyte, texture::rgb,      texture::rgb  > tex2ub;

	tex2f_msaa    *offscreen_rgbaTex;	// screen texture
	idTex_msaa    *offscreen_idTex;
	DepthTex_msaa *offscreen_depthTex;

	TextureCapture offscreen_frame;





	tex2f_msaa    *screenTex;	// screen texture
	idTex_msaa    *idTex;
	DepthTex_msaa *depthTex;

	tex2ub *screenDisplayTex;

	ScreenOverlay *screenOverlay;

	VoxelizerAsset *asset;

	//Camera
	gl::Camera camera;

	string selectedModel;
	string modelDir;


	ShaderProgram defaultShader;

	ShaderProgram depthVisShader;
	ShaderProgram idVisShader;

	ShaderProgram screenOverlayShader;

	bool dumpOffscreenFlag   = false;

	bool showDepth = false;

	int msaaSamples = 1;

	int xResMultipler = 2;
	int yResMultipler = 2;

	TextureCapture frame;

	GLuint screenVAO;

	bool fixedCam;

	void renderIMGUI();

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

	void dumpFrame();
	void dumpOffscreen();


	mat4f modelMat;
};


