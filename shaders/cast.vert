//*****************************************************************************
//cast.vert********************************************************************
//*****************************************************************************

#version 420

//layout(binding = 1, std140) uniform Matrices
//{
//	mat4 projMatrix;
//	mat4 viewMatrix;
//	mat4 modelMatrix;
//};

layout(location =  0) in vec3 position;
layout(location =  1) in vec3 normal;
layout(location =  2) in vec2 texCoord;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform mat4 unitToVoxel;

out block
{
	vec3 voxelPos;
	vec3 worldPos;
	vec3 vertexPos;
} Out;

void main()
{
	Out.worldPos = position.xyz;
	Out.voxelPos = (unitToVoxel * vec4(position,1.0)).xyz;

	Out.vertexPos = (projMatrix * viewMatrix * modelMatrix * vec4(position,1.0)).xyz;
	gl_Position   = (projMatrix * viewMatrix * modelMatrix * vec4(position,1.0));
}
