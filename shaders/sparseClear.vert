//*****************************************************************************
//sparseClear.vert*************************************************************
//*****************************************************************************

#version 420

layout(location = 0) in ivec3 activeVoxPos;

//Needs a manually added include file which defines the clear function
//and provides appropriate uniforms

void main()
{
	clearVoxels(activeVoxPos);
}

