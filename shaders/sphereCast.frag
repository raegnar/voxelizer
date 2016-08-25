//*****************************************************************************
//sphereCast.frag**************************************************************
//*****************************************************************************

#version 420

in block
{
	vec3 voxelPos;
	vec3 worldPos;
	vec3 vertexPos;
	vec2 texCoord;
	vec4 start;
} In;


uniform  vec3 rayOrigin;
uniform  vec3 lightPos;
uniform ivec3 volumeDim;
uniform  mat4 matVoxelToScreen;
uniform  int  mipLevel;

layout(location = 0) out vec4 fragOut;

bool castRay(vec3 origin, vec3 dir, out vec4 color, bool lines)
{
	const float fltMax = 3.402823466e+38;
	const float eps = exp2(-50.0);

	color = vec4(0.0);
	vec4 shColor;

	vec3 deltaT = 1.0 / dir;
	
	// determine intersection points with voxel grid box
	vec3 tBox0 = (vec3(0.0) - origin) * deltaT;
	vec3 tBox1 = (vec3(volumeDim) - origin) * deltaT;

	vec3 tBoxMax = max(tBox0, tBox1);
	vec3 tBoxMin = min(tBox0, tBox1);

	float tEnter = max(tBoxMin.x, max(tBoxMin.y, tBoxMin.z));
	float tExit  = min(tBoxMax.x, min(tBoxMax.y, tBoxMax.z));

	if(tEnter > tExit || tExit < 0.0)
		return false;

	deltaT = abs(deltaT);
	float t0 = max(tEnter - 0.5 * min(deltaT.x, min(deltaT.y, deltaT.z)), 0.0);		// start outside grid unless origin is inside

	vec3 pos = origin + t0 * dir;

	ivec3 cellStep;
	cellStep.x = (dir.x < 0.0) ? -1 : 1;
	cellStep.y = (dir.y < 0.0) ? -1 : 1;
	cellStep.z = (dir.z < 0.0) ? -1 : 1;

	ivec3 voxelCoord = ivec3(floor(pos));

	vec3 tMax = vec3(fltMax);
	if(dir.x > 0.0) tMax.x = ( float(voxelCoord.x + 1) - pos.x) * deltaT.x;
	if(dir.x < 0.0) tMax.x = (-float(voxelCoord.x + 0) + pos.x) * deltaT.x;
	if(dir.y > 0.0) tMax.y = ( float(voxelCoord.y + 1) - pos.y) * deltaT.y;
	if(dir.y < 0.0) tMax.y = (-float(voxelCoord.y + 0) + pos.y) * deltaT.y;
	if(dir.z > 0.0) tMax.z = ( float(voxelCoord.z + 1) - pos.z) * deltaT.z;
	if(dir.z < 0.0) tMax.z = (-float(voxelCoord.z + 0) + pos.z) * deltaT.z;

	// traverse voxel grid until ray hits a voxel or grid is left
	int maxSteps = volumeDim.x + volumeDim.y + volumeDim.z + 1;
	float t;
	vec3 tMaxPrev;
	vec4 cellColor;
	for(int i = 0; i < maxSteps; i++)
	{
		t = min(tMax.x, min(tMax.y, tMax.z));
		if(t0 + t >= tExit)
			return false;

		tMaxPrev = tMax; 
		if(tMax.x <= t) { tMax.x += deltaT.x; voxelCoord.x += cellStep.x; }
		if(tMax.y <= t) { tMax.y += deltaT.y; voxelCoord.y += cellStep.y; }
		if(tMax.z <= t) { tMax.z += deltaT.z; voxelCoord.z += cellStep.z; }

		if(any(        lessThan(voxelCoord.xyz, ivec3(0) )))	continue;
		if(any(greaterThanEqual(voxelCoord.xyz, volumeDim)))	continue;

	//	pos = origin + (t0 + t) * dir;
		if(isSphereInVoxelHit(voxelCoord, dir, rayOrigin, mipLevel, pos, color))
			break;
	}
	return true;
}


void main()
{
//	if(!gl_FrontFacing)	discard;

	vec3 dir = normalize(In.voxelPos - rayOrigin);

	if(!castRay(rayOrigin, dir, fragOut, false))
	{
		discard;
	}
}
