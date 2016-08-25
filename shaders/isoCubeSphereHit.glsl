//*****************************************************************************
//isoCubeSphereHit.glsl********************************************************
//*****************************************************************************

uniform  sampler3D Voxels;
//uniform  sampler3D  aTex;

bool isVoxelSet(in ivec3 coord, in vec3 pos, in vec3 dir, in int mipLevel, out vec4 color) 
{
	color = texelFetch(Voxels, coord, mipLevel);//.gbar;
	bool isSet = any(greaterThan(color, vec4(0)));
	color.a = 1.0f;
	return isSet;
}


bool isSphereInVoxelHit(ivec3 coord, vec3 dir, vec3 cam, in int mipLevel, out vec3 hit, out vec4 color)
{
	color = texelFetch(Voxels, coord, mipLevel);//.gbar;

	bool isSet = any(greaterThan(color, vec4(0)));
	color.a = 1.0f;

	if(isSet)
	{
		vec3 center = vec3(coord) + vec3(0.5f);

		float r = 0.5f;				//radius
		float a = dot(dir, dir);
		float b = 2.0f * dot(dir, cam - center);
		float c = dot(cam, cam) + dot(center, center) - 2.0f * dot(center, cam) - r*r;
		float test = b*b - 4.0*a*c;

		return (test >= 0.0);	//Hit
	}
	return false;
}