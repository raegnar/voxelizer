//*****************************************************************************
//denseClear.frag**************************************************************
//*****************************************************************************

#version 420

//Needs a manually added include file which defines the clearVoxels function
//and provides appropriate uniforms

flat in int instanceID;

void main()
{
	clearVoxels(ivec3(gl_FragCoord.xy,instanceID));
}
