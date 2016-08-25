//*****************************************************************************
//isoVoxelStore.glsl***********************************************************
//*****************************************************************************

//#extension GL_ARB_bindless_texture : enable			//doesn't seem to be necessary

#include "imageAtomicAverageRGBA8.glsl"

#define ATOMIC_MAX 0
#define ATOMIC_AVG 1

#define STORAGE_METHOD ATOMIC_MAX

#define ENABLED  0
#define DISABLED 1

#define VOXEL_BUFFER ENABLED

//remove previously "binding = x" seems unecesarry, also adding "bindless_image" to layout seems not to be required
layout(binding = 0, r32ui) coherent volatile uniform uimage3D Voxels;

#if VOXEL_BUFFER == ENABLED
layout(binding = 1, r32ui) uniform coherent volatile uimage3D     VoxelMutex;	//Mutex texture
layout(binding = 2, r32ui) uniform                   uimageBuffer VoxelBuffer;	//active voxels

//Atomic Counter binding points
layout(binding = 0) uniform atomic_uint currVoxCounter;
#endif

 
void imageStoreSelect(layout(r32ui) coherent volatile uimage3D voxels, ivec3 coord, vec3 color)
{
#if STORAGE_METHOD == ATOMIC_AVG
	imageAtomicAverageRGBA8Custom(voxels, coord, color.xyz);
#elif STORAGE_METHOD == ATOMIC_MAX
	imageAtomicMax(voxels, coord, packUnorm4x8(vec4(color.xyz,1)));
#endif
}

void storeVoxels(ivec3 currCoord, vec4 color, vec3 normal)
{
	imageStoreSelect(Voxels, currCoord, color.xyz);

#if VOXEL_BUFFER == ENABLED
	int index;
	if(imageAtomicCompSwap(VoxelMutex, currCoord, 0, 1) == 0)
	{
		index = 3*int(atomicCounterIncrement(currVoxCounter));
		imageStore(VoxelBuffer, index+0, uvec4(currCoord.x));
		imageStore(VoxelBuffer, index+1, uvec4(currCoord.y));
		imageStore(VoxelBuffer, index+2, uvec4(currCoord.z));
	}
#endif
}
