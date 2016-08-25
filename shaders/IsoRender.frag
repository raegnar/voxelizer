//*****************************************************************************
//IsoRender.frag***************************************************************
//*****************************************************************************

#version 420

#extension GL_ARB_explicit_uniform_location : enable

in block
{
	vec3 voxelPos;
	vec3 worldPos;
	vec2 texCoord;
	vec3 normal;
	vec3 normalWorld;
	vec3 tangent;
	vec3 bitangent;
} In;

layout(binding = 0, std140) uniform Material
{
	vec4  diffuse;
	vec4  ambient;
	vec4  specular;
	vec4  emissive;
	float shininess;
	int   texCount;
} material;

uniform sampler2D specularTex;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform float minVoxSize;
uniform float minVoxSizeInv;

uniform float diffuseConeRatio;
uniform float maxDist;
uniform float contribution;
uniform float shadowConeRatio;

uniform vec3 cameraPos;

uniform sampler3D Voxels;

layout(location = 0) out vec4 fragOut;

const vec3 dirLUT[6] =
{
	vec3( 0.000000, 1.000000, 0.000000),
	vec3( 0.000000, 0.500000, 0.866025),
	vec3( 0.823639, 0.500000, 0.267617),
	vec3( 0.509037, 0.500000,-0.700629),
	vec3(-0.509037, 0.500000,-0.700629),
	vec3(-0.823639, 0.500000, 0.267617)
};

const float weightLUT[6] = 
{
	0.7853981633974483096156608458198757210492923498437764,	//  pi/4
	0.4712388980384689857693965074919254326295754099062658,	// 3pi/20
	0.4712388980384689857693965074919254326295754099062658,	// 3pi/20
	0.4712388980384689857693965074919254326295754099062658,	// 3pi/20
	0.4712388980384689857693965074919254326295754099062658,	// 3pi/20
	0.4712388980384689857693965074919254326295754099062658,	// 3pi/20
};

// 3. Tomas Moller and John F. Hughes, "Efficiently building a matrix
//    to rotate one vector to another," Journal of Graphics Tools,
//    4(4):1-4, 1999.
mat3 buildRotation(vec3 from, vec3 to)
{
	float e = dot(from, to);
	vec3  v = normalize(cross(from, to));

	float h = (1.0f - e) / dot(v, v);

	mat3 mat;

	mat[0][0] = h*v.x*v.x + e;
	mat[0][1] = h*v.x*v.y - v.z;
	mat[0][2] = h*v.x*v.z + v.y;

	mat[1][0] = h*v.x*v.y + v.z;
	mat[1][1] = h*v.y*v.y + e;
	mat[1][2] = h*v.y*v.z - v.x;

	mat[2][0] = h*v.x*v.z - v.y;
	mat[2][1] = h*v.y*v.z + v.x;
	mat[2][2] = h*v.z*v.z + e;

	return mat;
}


//-----------------------------------------------------------------------------
//Shadows----------------------------------------------------------------------
//-----------------------------------------------------------------------------
float isoShadowFetch(vec3 pos, float lod)
{
	return textureLod(Voxels, pos, lod).w;
//	return float(any(greaterThan(textureLod(Voxels, pos, lod), vec4(0))));
}

float isoShadowTrace(vec3 origin, vec3 dir, float coneRatio, float maxDist)
{
	vec3  samplePos = origin;
	float accum = 0.0f;

	float minDiameter = minVoxSize;
	float startDist   = minDiameter;

	float dist = startDist;
	while(dist <= maxDist && accum < 1.0f)
	{
		float sampleDiameter = max(minDiameter, coneRatio * dist);
		float sampleLOD      = log2(sampleDiameter * minVoxSizeInv);
		vec3  samplePos      = origin + dir * dist;
		float sampleValue    = isoShadowFetch(samplePos, sampleLOD);
		float sampleWeight   = 1.0f - accum;
		accum += sampleValue * sampleWeight;
		dist  += sampleDiameter;
	}
	return accum;
}

//-----------------------------------------------------------------------------
//Voxels-----------------------------------------------------------------------
//-----------------------------------------------------------------------------
vec4 isoVoxelFetch(vec3 pos, float lod)
{
	return textureLod(Voxels, pos, lod);
}

vec4 isoVoxelConeTrace(vec3 origin, vec3 dir, float coneRatio, float maxDist)
{
	vec3 samplePos = origin;
	vec4 accum = vec4(0.0);

	float minDiameter = minVoxSize;
	float startDist   = minVoxSize;

	for(float dist = minVoxSize; dist <= maxDist && accum.w < 1.0;)
	{
		float sampleDiameter = max(minDiameter, coneRatio * dist);
		float sampleLOD      = log2(sampleDiameter * minVoxSizeInv);
		vec3  samplePos      = origin + dir * dist;
		vec4  sampleValue    = isoVoxelFetch(samplePos, sampleLOD);
		float sampleWeight   = 1.0f - accum.w;
//		if(sampleLOD > 1.0f) accum.xyz += sampleValue.xyz * sampleWeight;
		accum.xyz += sampleValue.xyz * sampleWeight;
		accum.w   += sampleValue.w   * sampleWeight;
		dist      += sampleDiameter;
	}

	return accum;
}

//-----------------------------------------------------------------------------
//Main-------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void main()
{
	vec4  diffuseSample  = texture2D( diffuseTex, In.texCoord);	if(diffuseSample.a < 0.5) discard;
	vec4  normalSample   = texture2D(  normalTex, In.texCoord);
	vec4  specularSample = texture2D(specularTex, In.texCoord);

	mat3 tbn = mat3(In.tangent, In.bitangent, In.normalWorld);

	vec3 tsNormal   = normalSample.xyz * 2.0 - 1.0;	//Tangent Space Normal
	vec3 bumpNormal = normalize(tbn * tsNormal);

	float intensity = clamp(dot(bumpNormal, lightDir), 0.0, 1.0);
	
	vec3 specularLight = vec3(0);
	{
		vec3 eyeToFragment = normalize(In.worldPos - cameraPos);
		vec3 reflectionDir = reflect(eyeToFragment, bumpNormal);
		float specConeRatio = .2;
		float specMaxDist = .3;

		specularLight = isoVoxelConeTrace(In.voxelPos, reflectionDir, specConeRatio, specMaxDist).xyz;
	}

	// Indirect Diffuse Contribution
	vec3 indirectDiffuse = vec3(0);
	{
		const float invN = 1.0f/6.0f;
		const float invPI = 1.0f/3.14159f;
		const float aoConeRatio = 1.15470053838; // corresponds to 60 degrees 
		mat3 vecTransform = buildRotation(dirLUT[0],-bumpNormal);	//build a rotation matrix to transform from normal to vec3(0,1,0)
		indirectDiffuse += vec3(invN * isoVoxelConeTrace(In.voxelPos, normalize(vecTransform * dirLUT[0]), diffuseConeRatio, maxDist) * weightLUT[0]);
		indirectDiffuse += vec3(invN * isoVoxelConeTrace(In.voxelPos, normalize(vecTransform * dirLUT[1]), diffuseConeRatio, maxDist) * weightLUT[1]);
		indirectDiffuse += vec3(invN * isoVoxelConeTrace(In.voxelPos, normalize(vecTransform * dirLUT[2]), diffuseConeRatio, maxDist) * weightLUT[2]);
		indirectDiffuse += vec3(invN * isoVoxelConeTrace(In.voxelPos, normalize(vecTransform * dirLUT[3]), diffuseConeRatio, maxDist) * weightLUT[3]);
		indirectDiffuse += vec3(invN * isoVoxelConeTrace(In.voxelPos, normalize(vecTransform * dirLUT[4]), diffuseConeRatio, maxDist) * weightLUT[4]);
		indirectDiffuse += vec3(invN * isoVoxelConeTrace(In.voxelPos, normalize(vecTransform * dirLUT[5]), diffuseConeRatio, maxDist) * weightLUT[5]);
	}

	// Soft Shadowing
	float shadow = 1.0f;
	{
		shadow -= isoShadowTrace(In.voxelPos, lightDir, shadowConeRatio, 1.0);
	}

	vec3 diffuseLight = lightColor * (intensity * shadow);

	fragOut.xyz = (diffuseLight + indirectDiffuse*contribution) * diffuseSample.xyz;
	fragOut.w = 1;


//	fragOut.xyz = In.normalWorld;
//	fragOut.xyz = bumpNormal;
//	fragOut.xyz = indirectDiffuse;

}

