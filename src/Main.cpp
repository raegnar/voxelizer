//*****************************************************************************
//Main.cpp*********************************************************************
//*****************************************************************************

//Standard Library Includes
#include <string>
#include <vector>

//Project Includes
#include "VoxelizerApp.h"


int main(int argc, char *argv[])
{
	std::vector<std::string> args;
	for(int i = 1; i < argc; i++)
	{
		args.push_back(string(argv[i]));
	}

	VoxelizerApp rasterApp("Voxelizer", 1280, 720, args);
	rasterApp.run();

	return 0;
}
