//*****************************************************************************
//DefaultRender.vert***********************************************************
//*****************************************************************************

#version 420

const int MAX_BONES = 100;

uniform mat4 bones[MAX_BONES];
uniform uint numBones;

//layout(binding = 1, std140) uniform Matrices
//{
//	mat4 projMatrix;
//	mat4 viewMatrix;
//	mat4 modelMatrix;
//};

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

// IN (from OpenGL)
layout(location = 0) in  vec3 position;
layout(location = 1) in  vec3 normalIn;
layout(location = 2) in  vec2 texCoord;
layout(location = 3) in  vec3 tangent;
layout(location = 4) in  vec3 bitangent;
//Animation stuff
layout(location = 5) in  uint boneIDs0;
layout(location = 6) in float boneWeights0;

out block
{
	vec3 voxelPos;
	vec3 worldPos;
	vec2 texCoord;
	vec3 normal;
	vec3 normalWorld;
	vec3 tangent;
	vec3 bitangent;
} Out;

out vec3 normal;

void main()
{ 
	vec4 vertexPos;
	if(numBones > 0)
	{
		mat4 boneMatrix = bones[boneIDs0] * boneWeights0;
		vec4 bonePos    = boneMatrix * vec4(position, 1);
		Out.normalWorld = vec3(boneMatrix * vec4(normalIn, 0));
		vertexPos = bonePos;
	}
	else
	{
		Out.normalWorld = normalIn;
		vertexPos = vec4(position, 1);
	}

	normal = normalIn;

	Out.tangent   = tangent;
	Out.bitangent = bitangent;

	Out.normal   = normalize(vec3(viewMatrix * modelMatrix * vec4(Out.normalWorld,0.0)));
	Out.texCoord = vec2(texCoord);

//	Out.worldPos = (modelMatrix * vertexPos).xyz;		//(-0.5 to 0.5)
	Out.worldPos = (modelMatrix * vertexPos).xyz;		//(-0.5 to 0.5)
	Out.voxelPos = Out.worldPos + vec3(0.5);

	gl_Position = projMatrix * viewMatrix * vec4(Out.worldPos,1);
}
