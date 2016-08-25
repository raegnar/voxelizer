//*****************************************************************************
//isoClear.glsl****************************************************************
//*****************************************************************************


layout(rgba8)  uniform  image3D Voxels;
layout(rgba8)  uniform  image3D aTex;
layout(r32ui)  uniform uimage3D Count;
layout(r32ui)  uniform uimage3D Mutex;

void clearVoxels(in ivec3 coord)
{
	imageStore(Voxels, coord,  vec4(0));
	imageStore(aTex,   coord,  vec4(0));
	imageStore(Count,  coord, uvec4(0));
	imageStore(Mutex,  coord, uvec4(0));
}

