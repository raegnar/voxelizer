//*****************************************************************************
//imageAtomicAverageRGBA8.glsl*************************************************
//*****************************************************************************

#ifndef IMAGE_ATOMIC_AVERAGE_RGBA8_GLSL
#define IMAGE_ATOMIC_AVERAGE_RGBA8_GLSL

vec4 unpackRGBCount(uint val)
{
	vec4 rgba;
	//mask of the approach quadrant of the uint then shift it to the end
	rgba.r = float((val & 0x000000FF)       );	//red
	rgba.g = float((val & 0x0000FF00) >>  8u);	//green
	rgba.b = float((val & 0x00FF0000) >> 16u);	//blue
	rgba.a = float((val & 0xFF000000) >> 24u);	//count
	return rgba;
}
//-----------------------------------------------------------------------------
uint packRGBCount(vec4 val)
{
	//Mask of the last 8 bits then shift them to the appropriate quadrant
	uint r = (uint(val.r) & 0x000000FF);
	uint g = (uint(val.g) & 0x000000FF) <<  8u;
	uint b = (uint(val.b) & 0x000000FF) << 16u;
	uint a = (uint(val.a) & 0x000000FF) << 24u;

	//OR the values together
	return (r | g | b | a);
}
//-----------------------------------------------------------------------------
void imageAtomicAverageRGBA8Custom(layout(r32ui) coherent volatile uimage3D voxels, ivec3 coord, vec3 nextVec3)
{
	uint nextUint = packRGBCount(vec4(nextVec3*255.0f,1));
	uint prevUint;
	uint currUint = 0;//packRGBCount(vec4(0,0,0,999));
	vec4 currVec4;

	vec3  average;
	uint  count;

	//Loop as long as destination value gets changed by other threads
	//compares currUint to nextUint
	while( ( prevUint = imageAtomicCompSwap(voxels, coord, currUint, nextUint) ) != currUint )
	{
		currUint = prevUint;					//store packed rgb average and count
		currVec4 = unpackRGBCount(currUint);	//unpack stored rgb average and count

		average =      currVec4.rgb / 255.0f;	//extract rgb average
		count   = uint(currVec4.a);				//extract count

		//Compute the running average
		average = (average*count + nextVec3) / float(count+1);

		//Pack new average and incremented count back into a uint
		nextUint = packRGBCount(vec4(average*255.0f, (count+1)));
	}
}


//-----------------------------------------------------------------------------
void imageAtomicAverageRGBA8Max(layout(r32ui) coherent volatile uimage3D voxels, ivec3 coord, vec3 nextVec3)
{
	uint nextUint = packRGBCount(vec4(1,nextVec3*255.0f));
	uint prevUint;
	uint currUint = 0;//packRGBCount(vec4(0,0,0,999));
	vec4 currVec4;

	vec3  average;
	uint  count;

	//Loop as long as destination value gets changed by other threads
	//compares currUint to nextUint
	while( ( prevUint = imageAtomicMax(voxels, coord, nextUint) ) != currUint )
	{
		currUint = prevUint;					//store packed rgb average and count
		currVec4 = unpackRGBCount(currUint);	//unpack stored rgb average and count

		average =      currVec4.gba / 255.0f;	//extract rgb average
		count   = uint(currVec4.r);				//extract count


		//Compute the running average
		average = (average*count + nextVec3) / float(count+1);

		//Pack new average and incremented count back into a uint
		nextUint = packRGBCount(vec4((count+1), average*255.0f));
	}
}


//-----------------------------------------------------------------------------
void imageAtomicAverageRGBA8Count(layout(r32ui) restrict volatile uimage3D voxels, layout(r32ui) restrict volatile uimage3D count, ivec3 coord, vec4 color)
{
	uint next = 1u;
	uint prev;
	uint curr = 0u;

	vec4 average;

	//Loop as long as destination value gets changed by other threads
	//compares curr to value in count at coord
	//if(count[coord] == curr) store next; return previous value at count[coord]
	//else return previous value at count[coord]
	while( ( prev = imageAtomicCompSwap(count, coord, curr, next) ) != curr )
	{
		//if prev == curr, that is count[coord] == curr
		curr = prev;
		next = curr+1;
	}

	average = unpackUnorm4x8(imageLoad(voxels, coord).x);
	average = (average*curr + color) / float(next);
	imageStore(voxels, coord, uvec4(packUnorm4x8(average)));

//	imageStore(count, coord, uvec4(0));
//	memoryBarrier();
}


//-----------------------------------------------------------------------------
void imageAtomicAverageRGBA8Count2(layout(r32ui) restrict volatile uimage3D voxels, layout(r32ui) restrict volatile uimage3D mutex, ivec3 coord, vec4 color)
{

	vec4 average;

	uint count = 0;
	
	uint isLockAvailable;

	do
	{
		isLockAvailable = imageAtomicCompSwap(mutex, coord, 0, 1);
		count += (1-isLockAvailable);
	}
	while(isLockAvailable == 0);

	average = unpackUnorm4x8(imageLoad(voxels, coord).x);
	average = (average*count + color) / float(count+1);
	imageStore(voxels, coord, uvec4(packUnorm4x8(average)));

	//unlock
	imageStore(mutex, coord, uvec4(0));

//	memoryBarrier();

}


//-----------------------------------------------------------------------------
//My Version adapted from Crassin version--------------------------------------
//-----------------------------------------------------------------------------
void imageAtomicAverageRGBA8(layout(r32ui) restrict  volatile uimage3D voxels, ivec3 coord, vec3 nextVec3)
{
	const float invRange = 1.0f/255.0f;
	uint nextUint = packUnorm4x8(vec4(nextVec3,invRange));
	uint prevUint = 0;
	uint currUint;
	vec4 currVec4;

	vec3  average;
	uint  count;

	//Loop as long as destination value gets changed by other threads
	//compares currUint to nextUint
	while( ( prevUint = imageAtomicCompSwap(voxels, coord, currUint, nextUint) ) != currUint )
	{
		currUint = prevUint;					//store packed rgb average and count
		currVec4 = unpackUnorm4x8(currUint);	//unpack stored rgb average and count

		average =      currVec4.rgb;			//extract rgb average
		count   = uint(currVec4.a*255.0f);		//extract count/255.0f

		//Compute the running average
		average = (average*count + nextVec3) / float(count+1);

		//Pack new average and incremented count back into a uint
		nextUint = packUnorm4x8(vec4(average, (count+1)*invRange));
	}
}


//-----------------------------------------------------------------------------
//Dx Converted Version-I found on the web somewhere----------------------------
//-----------------------------------------------------------------------------
//void InterlockedAverage(uint i0, uint i1, vec4 val)
void InterlockedAverage(layout( r32ui ) coherent volatile uimage3D voxels , ivec3 coord , vec4 val)
{
//	val.rgb *= 255.;
//	uint nval = Float4ToUint(val);
	uint nval = packUnorm4x8(val);
	uint prev = 0;
	uint current;

//	InterlockedCompareExchange(Nodes[i0].nodes[i1].data[1], prev, nval, current);
	current = imageAtomicCompSwap(voxels, coord, prev, nval);

	while(prev != current)
	{
		prev = current;
	//	float4 rval = UintToFloat4(current);
		vec4 rval = unpackUnorm4x8(current);
		rval.xyz *= rval.w;
	//	float4 curf = rval + val;
		vec4 curf = rval + val;
		curf.xyz /= curf.w;
	//	nval = Float4ToUint(curf);
		nval = packUnorm4x8(curf);
	//	InterlockedCompareExchange(Nodes[i0].nodes[i1].data[1], prev, nval, current);
		current = imageAtomicCompSwap(voxels, coord, prev, nval);
	}
}
//-----------------------------------------------------------------------------
//Crassin Version-----------------------------------------------------------
//-----------------------------------------------------------------------------
vec4 RGBA8toVec4(uint val)
{
	vec4 rgba;
	//mask of the approach quadrant of the uint then shift it to the end
	rgba.r = float((val & 0x000000FF)       );
	rgba.g = float((val & 0x0000FF00) >>  8u);
	rgba.b = float((val & 0x00FF0000) >> 16u);
	rgba.a = float((val & 0xFF000000) >> 24u);
	return rgba;
}
//-----------------------------------------------------------------------------
uint Vec4toRGBA8(vec4 val)
{
	//Mask of the last 8 bits then shift them to the appropriate quadrant
	uint r = (uint(val.r) & 0x000000FF);
	uint g = (uint(val.g) & 0x000000FF) <<  8u;
	uint b = (uint(val.b) & 0x000000FF) << 16u;
	uint a = (uint(val.a) & 0x000000FF) << 24u;

	//OR the values together
	return (r | g | b | a);
}
//-----------------------------------------------------------------------------
void imageAtomicAverageRGBA8Crassin(layout( r32ui ) coherent volatile uimage3D imgUI , ivec3 coords , vec4 val )
{
	val.rgb *= 255.0f; // Optimise following calculations
	uint newVal = Vec4toRGBA8(val);
	uint prevStoredVal = 0;
	uint curStoredVal;

	// Loop as long as destination value gets changed by other threads
	while( ( curStoredVal = imageAtomicCompSwap( imgUI , coords , prevStoredVal , newVal )) != prevStoredVal)
	{
		prevStoredVal = curStoredVal;
		volatile vec4 rval     = RGBA8toVec4(curStoredVal);
		rval.xyz      = ( rval.xyz * rval.w);	// Denormalize
		volatile vec4 curValF  = rval + val;			// Add new value
		curValF.xyz  /= (curValF.w);		// Renormalize
		newVal        = Vec4toRGBA8(curValF);
	}
}

#endif //IMAGE_ATOMIC_AVERAGE_RGBA8_GLSL
