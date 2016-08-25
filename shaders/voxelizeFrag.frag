//*****************************************************************************
//voxelizeFrag.frag************************************************************
//*****************************************************************************

#version 420


layout(binding = 0, std140) uniform Material
{
	vec4  diffuse;
	vec4  ambient;
	vec4  specular;
	vec4  emissive;
	float shininess;
	uint  hasDiffuseTex;
	uint  hasSpecularTex;
	uint  hasNormalTex;
	uint  texCount;
} material;

in block
{
	vec3 vertexPos;
	vec3 voxelPos;
	vec3 vertexNormal;
	vec2 texCoord;
} In;

uniform ivec3 volumeDim;

uniform sampler2D diffuseTex;

//OUT
layout(location = 0) out vec4 FragColor;

// IN voxels extents snapped to voxel grid (post swizzle)
flat in ivec3 minVoxIndex;
flat in ivec3 maxVoxIndex;

// IN 2D projected edge normals
flat in vec2 n_e0_xy;
flat in vec2 n_e1_xy;
flat in vec2 n_e2_xy;
flat in vec2 n_e0_yz;
flat in vec2 n_e1_yz;
flat in vec2 n_e2_yz;
flat in vec2 n_e0_zx;
flat in vec2 n_e1_zx;
flat in vec2 n_e2_zx;

// IN
flat in float d_e0_xy;
flat in float d_e1_xy;
flat in float d_e2_xy;
flat in float d_e0_yz;
flat in float d_e1_yz;
flat in float d_e2_yz;
flat in float d_e0_zx;
flat in float d_e1_zx;
flat in float d_e2_zx;

// IN pre-calculated triangle intersection stuff
flat in vec3  nProj;
flat in float dTriThin;
flat in float nzInv;

flat in int Z;

void main()
{
	if(any(greaterThan(In.vertexPos, maxVoxIndex)) || any(lessThan(In.vertexPos, minVoxIndex)))
	{
		discard;
		return;
	}

	ivec3 p = ivec3(In.vertexPos);	//voxel coordinate (swizzled)
	float zInt;						//voxel Z-intersection
	int   zMin,      zMax;			//voxel Z-range
	float zMinFloor, zMaxCeil;		//voxel Z-intersection floor/ceil

	float dd_e0_xy = d_e0_xy + dot(n_e0_xy, p.xy);
	float dd_e1_xy = d_e1_xy + dot(n_e1_xy, p.xy);
	float dd_e2_xy = d_e2_xy + dot(n_e2_xy, p.xy);

	bool xy_overlap = (dd_e0_xy >= 0) && (dd_e1_xy >= 0) && (dd_e2_xy >= 0);

	if(xy_overlap)
	{
		float dot_n_p = dot(nProj.xy, p.xy);
		zInt = (-dot_n_p + dTriThin) * nzInv;
		zMinFloor = floor(zInt);
		zMaxCeil  =  ceil(zInt);

		zMin = int(zMinFloor) - int(zMinFloor == zInt);
		zMax = int(zMaxCeil ) + int(zMaxCeil  == zInt);

		zMin = max(minVoxIndex.z, zMin);	//clamp to bounding box max Z
		zMax = min(maxVoxIndex.z, zMax);	//clamp to bounding box min Z

		for(p.z = zMin; p.z < zMax; p.z++)
		{
			float dd_e0_yz = d_e0_yz + dot(n_e0_yz, p.yz);
			float dd_e1_yz = d_e1_yz + dot(n_e1_yz, p.yz);
			float dd_e2_yz = d_e2_yz + dot(n_e2_yz, p.yz);
	
			float dd_e0_zx = d_e0_zx + dot(n_e0_zx, p.zx);
			float dd_e1_zx = d_e1_zx + dot(n_e1_zx, p.zx);
			float dd_e2_zx = d_e2_zx + dot(n_e2_zx, p.zx);
	
			bool yz_overlap = (dd_e0_yz >= 0) && (dd_e1_yz >= 0) && (dd_e2_yz >= 0);
			bool zx_overlap = (dd_e0_zx >= 0) && (dd_e1_zx >= 0) && (dd_e2_zx >= 0);

			if(yz_overlap && zx_overlap)
			{
				vec4 color = (material.hasDiffuseTex == 1u) ? texture2D(diffuseTex, In.texCoord) : material.diffuse; 

				ivec3 origCoord = (Z == 0) ? p.yzx : (Z == 1) ? p.zxy : p.xyz;	//this actually slightly outperforms unswizzle permutation matrix
				
				storeVoxels(origCoord, color, In.vertexNormal);
			}
		}
		//z-loop
	}
	//xy-overlap test

	discard;
}