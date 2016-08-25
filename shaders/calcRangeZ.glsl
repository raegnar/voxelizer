//*****************************************************************************
//calcRangeZ.glsl**************************************************************
//*****************************************************************************

#ifndef CALC_RANGE_Z_GLSL
#define CALC_RANGE_Z_GLSL

//calculate the Z-range for inner loop of intersection test, zMin & zMax must be initally assigned to min & max voxel-extents
void calcRangeZ(in vec3 n, in vec3 coord, in float dTriMin, in float dTriMax, in float nzInv, inout int zMin, inout int zMax)
{
	float zMinInt, zMaxInt, zMinFloor, zMaxFloor, dot_n_coord;

	dot_n_coord = dot(n.xy, coord.xy);

	zMinInt    = -(dot_n_coord + dTriMin) * nzInv;
	zMin       = int(max(zMin, floor(zMinInt)));
	
	zMaxInt    = -(dot_n_coord + dTriMax) * nzInv + 1.0;
	zMax       = int(min(zMax, floor(zMaxInt)));
}

#endif //CALC_RANGE_Z_GLSL
