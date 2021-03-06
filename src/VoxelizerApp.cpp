//*****************************************************************************
//TessApp.cpp*****************************************************************
//*****************************************************************************

//Header include
#include "VoxelizerApp.h"

// Project Includes
#include "VoxelAsset.h"

//Render Library Includes
#include <render/imgui.h>
#include <render/file.h>


#include "IsoVoxels.h"

//Standard Library Includes
#include <numeric>
#include <sstream>


//Namespaces
using namespace std;

namespace imgui = ImGui;

//*****************************************************************************
//Constructor******************************************************************
//*****************************************************************************
VoxelizerApp::VoxelizerApp(string title, int width, int height, const std::vector<std::string> &args) : App(title, width, height, true)
{

	string filename = (args.size() > 0) ? args[0] : "";

	cerr << "filename: " << filename << endl;

	int lastSlash = filename.find_last_of("/");
	selectedModel = filename.substr(lastSlash+1);
	modelDir = filename.substr(0, lastSlash+1);
	modelDir = (modelDir.size() < 1) ? "../subd/models" : modelDir;

	screenTex  = new    tex2f_msaa(App::width(), App::height()/*, msaaSamples*/);
	idTex      = new    idTex_msaa(App::width(), App::height()/*, msaaSamples*/);
	depthTex   = new DepthTex_msaa(App::width(), App::height()/*, msaaSamples*/);

	frame.attach(screenTex);
	frame.attach(idTex);
	frame.attachDepth(depthTex);

    voxels = new IsoVoxels(pow, width, height);


	modelMat.scale_by(vec3f(0.75f));

	screenDisplayTex = new tex2ub(width, height);

	screenOverlay = new ScreenOverlay();

	glGenVertexArrays(1, &screenVAO);

	asset = new VoxelAsset("models/crytek-sponza/sponza.obj");

	defaultShader.load(VertFile("./shaders/default.vert"), FragFile("./shaders/default.frag"));

	VertFile screenTriVertFile     = VertFile("./shaders/screenTri.vert");
	FragFile screenOverlayFragFile = FragFile("./shaders/screenOverlay.frag");

	screenOverlayShader.load(screenTriVertFile, screenOverlayFragFile);

	depthVisShader.load(screenTriVertFile, FragFile("./shaders/depthVis.frag"));
	idVisShader.load(   screenTriVertFile, FragFile("./shaders/idVis.frag"));


	initCam();

	fixedCam = false;

	ImGuiIO& io = imgui::GetIO();
	io.Fonts->AddFontFromFileTTF("DroidSans.ttf", 15.0f);

	ImGuiStyle& style = imgui::GetStyle();

	style.FramePadding  = {1,1};
	style.FrameRounding = 4.0f;
	style.ScrollbarRounding = 16.0f;
	style.ScrollbarWidth    = 16.0f;
	style.GrabRounding =  4.0f;
	style.GrabMinSize  = 10.0f;
	style.WindowRounding = 6.f;

	style.Colors[ImGuiCol_WindowBg        ] = {0,0,0,1.0f};
	style.Colors[ImGuiCol_TitleBg         ] = {0,0,0,0};
	style.Colors[ImGuiCol_TitleBgCollapsed] = {0,0,0,0};
	style.Colors[ImGuiCol_TitleBgActive   ] = {0,0,0,0};
	style.Colors[ImGuiCol_Header          ] = {0,0,0,0};
	style.Colors[ImGuiCol_HeaderHovered   ] = {1,0,0,1};
	style.Colors[ImGuiCol_HeaderActive    ] = {0,0,0,0};
	style.Colors[ImGuiCol_Button          ] = {.5,.5,.5,96./256.};
	style.Colors[ImGuiCol_ButtonHovered   ] = {.5,.5,.5,196./256.};
	style.Colors[ImGuiCol_ButtonActive    ] = {.5,.5,.5,196./256.};

//	style.Colors[ImGuiCol_ScrollbarBg         ] = {0,0,0,0};
	style.Colors[ImGuiCol_ScrollbarGrab       ] = {1.,1.,1., 96/256.};
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = {1.,196./256.,0, 96/256.};
	style.Colors[ImGuiCol_ScrollbarGrabActive ] = {1.,196./256.,0,196/256.};

	style.Colors[ImGuiCol_SliderGrabActive    ] = {230./256.,166./256.,0,1.0f};
}
//*****************************************************************************
//Initialize*******************************************************************
//*****************************************************************************
void VoxelizerApp::init()
{
}
//*****************************************************************************
//Render***********************************************************************
//*****************************************************************************
void VoxelizerApp::render()
{
	//Clear the RGB buffer and the depth buffer
	glClearColor(.5,.5,.5,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	camera.set_matrices();
	camera.render();

	glEnable(GL_CULL_FACE);

	glDisable(GL_ALPHA);
	frame.begin();
	{
		asset->render(camera.proj_mat, camera.view_mat, mat4f());
	}
	frame.end();


	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);


    if(voxelizationMethod == fragVoxelize)
    {
        voxels->voxelize(asset, camera);
    }
    if(voxelizationMethod == hybridVoxelize)
    {
        voxels->hybridVoxelize(asset, camera);
    }
    if(voxelizationMethod == persistentVoxelize)
    {
        voxels->persistentVoxelize(asset, camera);
    }





    if(showDepth)
	{
		screenOverlay->render(idTex);
	}
	else
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frame.id());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Normal FBO can be the default FBO too.
		glBlitFramebuffer(0, 0, screenTex->w, screenTex->h,
						  0, 0, App::width(), App::height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}



	glPrimitiveRestartIndex(0xFFFF);
	glEnable(GL_PRIMITIVE_RESTART);


	renderIMGUI();
}
//*****************************************************************************
//Events***********************************************************************
//*****************************************************************************
void VoxelizerApp::resize()
{
//	camera.set_viewport(vec2i(w, h));
	camera.set_viewport(vec2i(w, h));
}
//-----------------------------------------------------------------------------
void VoxelizerApp::mousemove()
{
	if(imgui::IsAnyItemActive())
		return;

	float dt = (float)this->dt();
	float modifier = 10000.0f;

	if (mouseDown(MouseButton::LEFT))
	{
		camera.mouse_move(dt, dx, dy, modifier);
	}
}
//*****************************************************************************
//Update***********************************************************************
//*****************************************************************************
void VoxelizerApp::update()
{
	float dt = (float)this->dt();

	float modifier = 1.0f;
	modifier = keyDown(Key::LSHIFT) ? 0.1f : 1.0f;

	modifier *= 1000.0f;

	if (fixedCam == false)
	{
		if (keyDown(Key::W))		{ camera.forward(dt, modifier*0.1f);      }
		if (keyDown(Key::S))		{ camera.back(dt, modifier*0.1f);         }
		if (keyDown(Key::Q))		{ camera.strafe_left(dt, modifier*.01f);  }
		if (keyDown(Key::E))		{ camera.strafe_right(dt, modifier*.01f); }
		if (keyDown(Key::P))		initCam();
	}

	if (keyDown(Key::SPACE))
	{
		camera.stop();
	}
	if (keyPressed(Key::ESCAPE))	exit(1);
	if (keyPressed(Key::R))			ShaderPrograms::refresh();

	frame.clear();
}
//*****************************************************************************
//Miscellaneous****************************************************************
//*****************************************************************************
void VoxelizerApp::initCam()
{
	camera.pos(vec3f(0.0f, 0.0f,  2));
	camera.dir(vec3f(0.0f, 0.0f, -1));
	camera.yaw(100);
	camera.roll(100);
	camera.pitch(100);
	camera.strafe(20);
	camera.velocity(.5);
//	camera.set_viewport(vec2i(w, h));
	camera.set_viewport(vec2i(w, h));
	camera.setNearFar(0.01f, 10.0f);
}

#define IM_ARRAYSIZE(_ARR)      ((int)(sizeof(_ARR)/sizeof(*_ARR)))

//*****************************************************************************
// Render Immediate Mode GUI **************************************************
//*****************************************************************************

void VoxelizerApp::renderIMGUI()
{
    //Set the viewport size
    glViewport(0, 0, w, h);

	imgui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_FirstUseEver);

	ImGui::SetNextWindowPos(ImVec2(10,10));
	bool overlayOpened = true;
	if (!ImGui::Begin("Example: Fixed Overlay", &overlayOpened, ImVec2(0,0), 0.3f, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::End();
		return;
	}

	imgui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / imgui::GetIO().Framerate, imgui::GetIO().Framerate);
	ImGui::End();

	imgui::Begin("Voxelizer");


    if(imgui::SliderInt("pow", &pow, 0, 9))
    {
        //change voxel dimensions
    }







    imgui::Text("On  Screen Res %i x %i", App::width(), App::height());


	const char* resMultiples[] = { "1", "2", "4", };

	static int xResIndex = 1;
	static int yResIndex = 1;

	bool resChanged = false;

	resChanged = imgui::Combo("xRes Mul", &xResIndex, resMultiples, IM_ARRAYSIZE(resMultiples)) || resChanged;
	resChanged = imgui::Combo("yRes Mul", &yResIndex, resMultiples, IM_ARRAYSIZE(resMultiples)) || resChanged;

	imgui::Separator();

	imgui::Separator();


    if(imgui::CollapsingHeader("Voxel Type"))
    {

    }


        imgui::Checkbox("Show Depth", &showDepth);



	imgui::Separator();

//	if(imgui::DragInt("MSAA Samples", &msaaSamples, 1.f, 1, 16))
//	{
//		screenTex->setSamples(msaaSamples);
//		depthTex->setSamples(msaaSamples);
//	}


	imgui::DragFloat3("EYE", &(camera._pos.x), 0.01f);
	imgui::DragFloat3("DIR", &(camera._dir.x), 0.01f);

	imgui::DragFloat("Near Plane", &(camera.near_plane), 0.01f);
	imgui::DragFloat("Far Plane",  &(camera.far_plane),  0.01f);

	imgui::DragFloat("FOV", &(camera.fov),  0.01f);

	imgui::DragMat44("View",  &(camera.view_mat.m00), 0.01f);
	imgui::DragMat44("Proj",  &(camera.proj_mat.m00), 0.01f);
	imgui::DragMat44("Model", &(modelMat.m00), 0.01f);

	if(imgui::Button("Load Model"))	imgui::OpenPopup("Load Model");
	ImGui::SameLine();
	ImGui::Text((selectedModel.size() > 0) ? selectedModel.c_str() : "<None>");
	if(imgui::BeginPopup("Load Model"))
	{
		vector<string> objFiles = file::listDir(modelDir.c_str(), "obj");
		vector<string> scnFiles = file::listDir(modelDir.c_str(), "scn");

		objFiles.insert(objFiles.begin(), scnFiles.begin(), scnFiles.end());


		for(int i = 0; i < objFiles.size(); i++)
		{
			if(imgui::Selectable(objFiles[i].c_str()))
			{
				string modelString = modelDir + "/" + objFiles[i];
				selectedModel = (char *)modelString.c_str();

				if(asset) delete asset;

				asset = new VoxelAsset(selectedModel);
			}
		}
		imgui::EndPopup();
	}


	imgui::Separator();

    imgui::Text("Voxelization Method");

    imgui::Separator();

    imgui::RadioButton("Frag-Voxelize",  (int *)&voxelizationMethod, fragVoxelize);
    imgui::RadioButton("Tri-Voxelize",   (int *)&voxelizationMethod, triVoxelize);
    imgui::RadioButton("Hybrid-Voxelize",(int *)&voxelizationMethod, hybridVoxelize);

	imgui::Separator();

	imgui::End();

	imgui::Render();

}

