//*****************************************************************************
//renderVoxels.frag************************************************************
//*****************************************************************************

#version 420

struct LightInfo 
{
	vec4 Position;	// Light position in eye coords.
	vec3 Intensity;	// A,D,S intensity
};

struct MaterialInfo
{
	vec3  Ka;			// Ambient reflectivity
	vec3  Kd;			// Diffuse reflectivity
	vec3  Ks;			// Specular reflectivity
	float Shininess;	// Specular shininess factor
};


uniform LightInfo    Light;
uniform MaterialInfo Material;

uniform vec3 boxDim;
uniform vec3 gridRes;

in block
{
	      vec3 vertexPos;
	      vec3 vertexNormal;
	flat  vec3 voxelColor;
	flat ivec3 voxelCoord;
} In;



layout (location = 0) out vec4 FragColor;




vec3 phongModel( vec3 pos, vec3 norm )
{
	vec3 s = normalize(vec3(Light.Position) - pos);
	vec3 v = normalize(-pos.xyz);
	vec3 r = reflect( -s, norm );
	vec3 ambient = Light.Intensity * Material.Ka;
	float sDotN = max( dot(s,norm), 0.0 );
	vec3 diffuse = Light.Intensity * Material.Kd * sDotN;
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 )
		spec = Light.Intensity * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess );
	return ambient + diffuse + spec;
}


vec3 phongModelDiffuse( vec3 pos, vec3 norm, vec3 Kd )
{
	vec3 s = normalize(vec3(Light.Position) - pos);
	vec3 v = normalize(-pos.xyz);
	vec3 r = reflect( -s, norm );
	vec3 ambient = Light.Intensity * Material.Ka;
	float sDotN = max( dot(s,norm), 0.0 );

	vec3 diffuse;
	if(Kd == vec3(0))
	{
		diffuse = Light.Intensity * Material.Kd * sDotN;
	}
	else
	{
		diffuse = Light.Intensity * Kd * sDotN;
	}
	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 )
		spec = Light.Intensity * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess );
	return ambient + diffuse + spec;
}



void main()
{
//	uint count = texelFetch(Voxels, In.voxelCoord, 0).x;

	vec3 finalColor = phongModelDiffuse(In.vertexPos, In.vertexNormal, In.voxelColor);

//	FragColor = vec4(phongModel(Position, vertexNormal), 1.0);

//	FragColor = vec4(finalColor/ float(count), 1.0 );
	FragColor = vec4(finalColor, 1.0 );
}
