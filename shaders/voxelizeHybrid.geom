//*****************************************************************************
//voxelizeHybrid.geom**********************************************************
//*****************************************************************************

#version 420

#define L     0.7071067811865475244008443621048490392848359376884740	//sqrt(2)/2
#define L_SQR 0.5


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in block
{
	vec3 vertexPos;
	vec3 vertexNormal;
	vec2 texCoord;
	int  vertexID;
} In[];


#include "swizzle.glsl"


//Small Triangle -- write to these
//layout(r32ui, binding = 0)                   uniform uimage3D Voxels;
//layout(r32ui, binding = 1) coherent volatile uniform uimage3D Mutex;		//Mutex texture

//Big Triangle -- write to these
layout(binding = 4) uniform writeonly uimageBuffer largeIdx;
layout(binding = 5) uniform writeonly uimageBuffer largeIndirectElement;

//Large Triangle Atomic Counter
layout(binding = 1) uniform atomic_uint largeTriCount;	//voxCounter is binding = 0 in the voxel specific include

uniform ivec3 volumeDim;
uniform float cutoff;


float triArea2D(vec2 v0, vec2 v1, vec2 v2)
{
	return abs(v0.x*(v1.y-v2.y) + v1.x*(v2.y-v0.y) + v2.x*(v0.y-v1.y)) * 0.5f;
}

void main()
{
	vec3 n;
	mat3 unswizzle;
	vec3 v0 = In[0].vertexPos;
	vec3 v1 = In[1].vertexPos;
	vec3 v2 = In[2].vertexPos;
	
	swizzleTri(v0, v1, v2, n, unswizzle);

//	atomicCounterIncrement(largeTriCount);//DEBUG

	
	if(triArea2D(v0.xy, v1.xy, v2.xy) >= cutoff)
	{
		//Store large triangles
		int index = int(atomicCounterIncrement(largeTriCount));
		imageStore(largeIdx, 3*index+0, uvec4(In[0].vertexID));
		imageStore(largeIdx, 3*index+1, uvec4(In[1].vertexID));
		imageStore(largeIdx, 3*index+2, uvec4(In[2].vertexID));

	//	imageStore(largeIdx, int(atomicCounterIncrement(largeTriCount)), uvec4(In[0].vertexID));
	//	imageStore(largeIdx, int(atomicCounterIncrement(largeTriCount)), uvec4(In[1].vertexID));
	//	imageStore(largeIdx, int(atomicCounterIncrement(largeTriCount)), uvec4(In[2].vertexID));
	}
	else
	{
		vec3 AABBmin = min(min(v0, v1), v2);
		vec3 AABBmax = max(max(v0, v1), v2);

		ivec3 minVoxIndex = ivec3(clamp(floor(AABBmin), ivec3(0), volumeDim));
		ivec3 maxVoxIndex = ivec3(clamp( ceil(AABBmax), ivec3(0), volumeDim));

		//Voxelize small triangles
		vec3 e0 = v1 - v0;	//figure 17/18 line 2
		vec3 e1 = v2 - v1;	//figure 17/18 line 2
		vec3 e2 = v0 - v2;	//figure 17/18 line 2

		vec3 u = vec3(1,0,0);
		vec3 v = vec3(0,1,0);
		vec3 w = vec3(0,0,1);

		//INward Facing edge normals XY
		vec2 n_e0_xy = (n.z >= 0) ? vec2(-e0.y, e0.x) : vec2(e0.y, -e0.x);	//figure 17/18 line 4
		vec2 n_e1_xy = (n.z >= 0) ? vec2(-e1.y, e1.x) : vec2(e1.y, -e1.x);	//figure 17/18 line 4
		vec2 n_e2_xy = (n.z >= 0) ? vec2(-e2.y, e2.x) : vec2(e2.y, -e2.x);	//figure 17/18 line 4

		//INward Facing edge normals YZ
		vec2 n_e0_yz = (n.x >= 0) ? vec2(-e0.z, e0.y) : vec2(e0.z, -e0.y);	//figure 17/18 line 5
		vec2 n_e1_yz = (n.x >= 0) ? vec2(-e1.z, e1.y) : vec2(e1.z, -e1.y);	//figure 17/18 line 5
		vec2 n_e2_yz = (n.x >= 0) ? vec2(-e2.z, e2.y) : vec2(e2.z, -e2.y);	//figure 17/18 line 5

		//INward Facing edge normals ZX
		vec2 n_e0_zx = (n.y >= 0) ? vec2(-e0.x, e0.z) : vec2(e0.x, -e0.z);	//figure 17/18 line 6
		vec2 n_e1_zx = (n.y >= 0) ? vec2(-e1.x, e1.z) : vec2(e1.x, -e1.z);	//figure 17/18 line 6
		vec2 n_e2_zx = (n.y >= 0) ? vec2(-e2.x, e2.z) : vec2(e2.x, -e2.z);	//figure 17/18 line 6

		float d_e0_xy = dot(n_e0_xy, .5-v0.xy) + 0.5 * max(abs(n_e0_xy.x), abs(n_e0_xy.y));	//figure 18 line 7
		float d_e1_xy = dot(n_e1_xy, .5-v1.xy) + 0.5 * max(abs(n_e1_xy.x), abs(n_e1_xy.y));	//figure 18 line 7
		float d_e2_xy = dot(n_e2_xy, .5-v2.xy) + 0.5 * max(abs(n_e2_xy.x), abs(n_e2_xy.y));	//figure 18 line 7

		float d_e0_yz = dot(n_e0_yz, .5-v0.yz) + 0.5 * max(abs(n_e0_yz.x), abs(n_e0_yz.y));	//figure 18 line 8
		float d_e1_yz = dot(n_e1_yz, .5-v1.yz) + 0.5 * max(abs(n_e1_yz.x), abs(n_e1_yz.y));	//figure 18 line 8
		float d_e2_yz = dot(n_e2_yz, .5-v2.yz) + 0.5 * max(abs(n_e2_yz.x), abs(n_e2_yz.y));	//figure 18 line 8

		float d_e0_zx = dot(n_e0_zx, .5-v0.zx) + 0.5 * max(abs(n_e0_zx.x), abs(n_e0_zx.y));	//figure 18 line 9
		float d_e1_zx = dot(n_e1_zx, .5-v1.zx) + 0.5 * max(abs(n_e1_zx.x), abs(n_e1_zx.y));	//figure 18 line 9
		float d_e2_zx = dot(n_e2_zx, .5-v2.zx) + 0.5 * max(abs(n_e2_zx.x), abs(n_e2_zx.y));	//figure 18 line 9

		vec3 nProj = (n.z < 0.0) ? -n : n;	//figure 17/18 line 10

		const float dTri     = dot(nProj, v0);
		const float dTriThin = dTri - dot(nProj.xy, vec2(0.5));	//figure 18 line 11

		const float nzInv = 1.0 / nProj.z;
	
		ivec3 p;					//voxel coordinate
		int   zMin,      zMax;		//voxel Z-range
		float zMinInt,   zMaxInt;	//voxel Z-intersection min/max
		float zMinFloor, zMaxCeil;	//voxel Z-intersection floor/ceil
		for(p.x = minVoxIndex.x; p.x < maxVoxIndex.x; p.x++)	//figure 17 line 13, figure 18 line 12
		{
			for(p.y = minVoxIndex.y; p.y < maxVoxIndex.y; p.y++)	//figure 17 line 14, figure 18 line 13
			{
				float dd_e0_xy = d_e0_xy + dot(n_e0_xy, p.xy);
				float dd_e1_xy = d_e1_xy + dot(n_e1_xy, p.xy);
				float dd_e2_xy = d_e2_xy + dot(n_e2_xy, p.xy);
		
				bool xy_overlap = (dd_e0_xy >= 0) && (dd_e1_xy >= 0) && (dd_e2_xy >= 0);

				if(xy_overlap)	//figure 17 line 15, figure 18 line 14
				{
					float dot_n_p = dot(nProj.xy, p.xy);

					zMinInt = (-dot_n_p + dTriThin) * nzInv;
					zMaxInt = zMinInt;

					zMinFloor = floor(zMinInt);
					zMaxCeil  =  ceil(zMaxInt);

					zMin = int(zMinFloor) - int(zMinFloor == zMinInt);
					zMax = int(zMaxCeil ) + int(zMaxCeil  == zMaxInt);

					zMin = max(minVoxIndex.z, zMin);	//clamp to bounding box max Z
					zMax = min(maxVoxIndex.z, zMax);	//clamp to bounding box min Z

					for(p.z = zMin; p.z < zMax; p.z++)	//figure 17/18 line 18
					{
						float dd_e0_yz = d_e0_yz + dot(n_e0_yz, p.yz);
						float dd_e1_yz = d_e1_yz + dot(n_e1_yz, p.yz);
						float dd_e2_yz = d_e2_yz + dot(n_e2_yz, p.yz);

						float dd_e0_zx = d_e0_zx + dot(n_e0_zx, p.zx);
						float dd_e1_zx = d_e1_zx + dot(n_e1_zx, p.zx);
						float dd_e2_zx = d_e2_zx + dot(n_e2_zx, p.zx);

						bool yz_overlap = (dd_e0_yz >= 0) && (dd_e1_yz >= 0) && (dd_e2_yz >= 0);
						bool zx_overlap = (dd_e0_zx >= 0) && (dd_e1_zx >= 0) && (dd_e2_zx >= 0);

						if(yz_overlap && zx_overlap)	//figure 17/18 line 19
						{
						//	writeVoxels(ivec3(unswizzle*p), 1, vec3(1));	

							storeVoxels(ivec3(unswizzle*p), vec4(1), vec3(1));	//figure 17/18 line 20
						}
					}
					//z-loop
				}
				//xy-overlap test
			}
			//y-loop
		}
		//x-loop
	}

	memoryBarrier();
	imageStore(largeIndirectElement, 0, uvec4(3*atomicCounter(largeTriCount)));
//	imageStore(largeIndirectElement, 0, uvec4(atomicCounter(largeTriCount)));

}
