//*****************************************************************************
//DefaultRender.frag***********************************************************
//*****************************************************************************

#version 420

#extension GL_ARB_explicit_uniform_location : enable

//layout(early_fragment_tests) in;

layout(binding = 0, std140) uniform Material
{
	vec4  diffuse;
	vec4  ambient;
	vec4  specular;
	vec4  emissive;
	float shininess;
	uint  hasDiffuseTex;
	uint  hasSpecularTex;
	uint  hasNormalTex;
	uint  texCount;
} material;


uniform sampler2D specularTex;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 cameraPos;

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

layout(location = 0) out vec4 diffOut;		//diffuse color out
layout(location = 1) out vec4 specOut;		//diffuse color out
layout(location = 2) out vec4 vposOut;		//voxel position out (0.0 to 1.0)
layout(location = 3) out vec4 bumpOut;		//bump normal out
layout(location = 4) out vec4 surfOut;		//surface normal out

mat3 cotangentFrame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1  = dFdx( p );
    vec3 dp2  = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturbNormal( vec3 n, vec3 v, vec3 mapNormal)
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
//    vec3 map = texture2D( mapBump, texcoord ).xyz;
//#ifdef WITH_NORMALMAP_UNSIGNED
//    map = map * 255./127. - 128./127.;
//#endif
//#ifdef WITH_NORMALMAP_2CHANNEL
//    mapNormal.z = sqrt( 1. - dot( mapNormal.xy, mapNormal.xy ) );
//#endif
//#ifdef WITH_NORMALMAP_GREEN_UP
    mapNormal.y = -mapNormal.y;
//#endif
    mat3 tbn = cotangentFrame( n, -v, In.texCoord );
    return normalize( tbn * mapNormal );
}

void main()
{
	vec4 diffuseSample  = texture2D( diffuseTex, In.texCoord);	if(diffuseSample.a < 0.5) discard;
	vec4 specularSample = texture2D(specularTex, In.texCoord);
	vec4 normalSample   = texture2D(  normalTex, In.texCoord)*2-1;

	vec3 view = normalize(cameraPos - In.worldPos);

	vec3 bumpNormal = perturbNormal(normalize(In.normal), view, normalSample.xyz);

// OLD OLD OLD
//	mat3 tbn = mat3(In.tangent, In.bitangent, In.normal);
//	vec3 tsNormal   = normalSample.xyz * 2.0 - 1.0;	//Tangent Space Normal
//	vec3 bumpNormal = normalize(tbn * tsNormal);


	float intensity = clamp(dot(bumpNormal, lightDir), 0.0, 1.0);
	
	vec3 diffuseLight = lightColor * (intensity);


	diffOut = (material.hasDiffuseTex == 1u) ? diffuseSample : material.diffuse;

//	diffOut.xyz = vec3(material.shininess/50);
//	diffOut.w   = 1.0f;

//	diffOut = vec4(In.bitangent, 1.0f);

	specOut = (material.hasSpecularTex == 1u) ? specularSample : vec4(0,0,0,1);

	vposOut.xyz = In.voxelPos;
	vposOut.w   = 1.0f;

	surfOut.xyz = normalize(In.normal);
	surfOut.w   = 1.0f;

	bumpOut = (material.hasNormalTex == 1u) ? vec4(bumpNormal.xyz,1) : surfOut;

}

/*
layout(early_fragment_tests) in;
in vec3 normal;
layout(binding = 0) writeonly uniform image2D outputTex;
void main()
{
	vec4 fragColor = vec4(normal, 1);
	imageStore(outputTex, ivec2(gl_FragCoord.xy), fragColor);
}
*/
