//*****************************************************************************
//DefaultRender.geom***********************************************************
//*****************************************************************************

#version 420

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in block
{
	vec3 voxelPos;
	vec3 worldPos;
	vec2 texCoord;
	vec3 normal;
	vec3 normalWorld;
	vec3 tangent;
	vec3 bitangent;
} In[];


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


void main()
{
	vec3 v0 = In[0].worldPos;
	vec3 v1 = In[1].worldPos;
	vec3 v2 = In[2].worldPos;

	//                 cross(e0, e1);
	vec3 n = normalize(cross(v1 - v0, v2 - v1));

	gl_Position     = gl_in[0].gl_Position;
	Out.voxelPos    = In[0].voxelPos;
	Out.worldPos    = In[0].worldPos;
	Out.texCoord    = In[0].texCoord;
	Out.normal      = In[0].normalWorld;
	Out.normalWorld = n;
	Out.tangent     = In[0].tangent;
	Out.bitangent   = In[0].bitangent;
	EmitVertex();

	gl_Position     = gl_in[1].gl_Position;
	Out.voxelPos    = In[1].voxelPos;
	Out.worldPos    = In[1].worldPos;
	Out.texCoord    = In[1].texCoord;
	Out.normal      = In[1].normalWorld;
	Out.normalWorld = n;
	Out.tangent     = In[1].tangent;
	Out.bitangent   = In[1].bitangent;
	EmitVertex();

	gl_Position     = gl_in[2].gl_Position;
	Out.voxelPos    = In[2].voxelPos;
	Out.worldPos    = In[2].worldPos;
	Out.texCoord    = In[2].texCoord;
	Out.normal      = In[2].normalWorld;
	Out.normalWorld = n;
	Out.tangent     = In[2].tangent;
	Out.bitangent   = In[2].bitangent;
	EmitVertex();

	EndPrimitive();
}

