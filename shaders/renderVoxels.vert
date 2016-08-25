//*****************************************************************************
//renderVoxels.vert************************************************************
//*****************************************************************************


#version 420

layout(location = 0) in  vec3 VertexPosition;
layout(location = 1) in  vec3 VertexNormal;
layout(location = 2) in  vec2 TextureCoord;

layout(location = 3) in ivec3 VoxelCoord;

out block
{
	      vec3 vertexPos;
	      vec3 vertexNormal;
	flat  vec3 voxelColor;
	flat ivec3 voxelCoord;
} Out;


uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 boxDim;
uniform vec3 gridRes;

void main()
{
	vec3 WorldCoord  = VertexPosition * boxDim;

	Out.voxelCoord = VoxelCoord;
//	Out.voxelCoord = ivec4(unpackUnorm4x8(VoxelCoord)).yzw;

	vec3 VoxelCenter = (VoxelCoord - gridRes/2 + vec3(0.5)) / gridRes;
//	if(VoxelCoord.w == 0)
		WorldCoord += VoxelCenter;

	Out.vertexPos = (viewMatrix * vec4(WorldCoord, 1.0)).xyz;// + VoxelNormal;

	Out.vertexNormal   = normalize(vec3(viewMatrix * vec4(VertexNormal,0.0)));
	
	// Draw at the current position
	gl_Position = projMatrix * vec4(Out.vertexPos, 1.0);
}
