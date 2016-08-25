//*****************************************************************************
//cubeCast.frag****************************************************************
//*****************************************************************************

#version 420

in block
{
	vec3 voxelPos;
	vec3 worldPos;
	vec3 vertexPos;
} In;


uniform vec3  rayOrigin;
uniform int   mipLevel;
uniform ivec3 volumeDim;

layout(location = 0) out vec4 fragOut;

bool castRay(vec3 origin, vec3 dir, out vec4 outColor, bool lines)
{
	const float fltMax = 3.402823466e+38;
	const float eps = exp2(-50.0);

	outColor = vec4(0.0);

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
	if(dir.x > 0.0) tMax.x = (float(voxelCoord.x + 1) - pos.x) * deltaT.x;
	if(dir.x < 0.0) tMax.x = (pos.x - float(voxelCoord.x)) * deltaT.x;
	if(dir.y > 0.0) tMax.y = (float(voxelCoord.y + 1) - pos.y) * deltaT.y;
	if(dir.y < 0.0) tMax.y = (pos.y - float(voxelCoord.y)) * deltaT.y;
	if(dir.z > 0.0) tMax.z = (float(voxelCoord.z + 1) - pos.z) * deltaT.z;
	if(dir.z < 0.0) tMax.z = (pos.z - float(voxelCoord.z)) * deltaT.z;

	// traverse voxel grid until ray hits a voxel or grid is left
	int maxSteps = volumeDim.x + volumeDim.y + volumeDim.z + 1;
	float t;
	vec4 color;
	vec3 tMaxPrev, n;
	for(int i = 0; i < maxSteps; i++)
	{
		t = min(tMax.x, min(tMax.y, tMax.z));
		if(t0 + t >= tExit)
			return false;

		n = vec3(0);
		tMaxPrev = tMax; 
		if(tMax.x <= t) { tMax.x += deltaT.x; voxelCoord.x += cellStep.x;  n.x = dir.x > 0.0 ? -1.0 : 1.0;}
		if(tMax.y <= t) { tMax.y += deltaT.y; voxelCoord.y += cellStep.y;  n.y = dir.y > 0.0 ? -1.0 : 1.0;}
		if(tMax.z <= t) { tMax.z += deltaT.z; voxelCoord.z += cellStep.z;  n.z = dir.z > 0.0 ? -1.0 : 1.0;}

		if(any(        lessThan(voxelCoord.xyz, ivec3(0) )))	continue;
		if(any(greaterThanEqual(voxelCoord.xyz, volumeDim)))	continue;

		n = normalize(n);
		pos = origin + (t0 + t) * dir;
		if(isVoxelSet(voxelCoord, pos, n, mipLevel, color))
		{
			outColor.rgb = (color.rgb*color.a) + outColor.rgb*(1-color.a);
			outColor.a += color.a;
			if(outColor.a >= 1.0f)
				break;
		}
	}
	return true;
}


void main()
{
	vec3 dir = normalize(In.voxelPos - rayOrigin);

	bool hit = castRay(rayOrigin, dir, fragOut, false);

	if(!hit)
	{
		discard;
	}
}
