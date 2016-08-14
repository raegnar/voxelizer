

#version 410

layout (std140) uniform Material
{
	vec4  diffuse;
	vec4  ambient;
	vec4  specular;
	vec4  emissive;
	float shininess;
	int   texCount;
};

uniform sampler2D texUnit;

     in vec3 normal;
     in vec2 texCoord;

//in int gl_PrimitiveID;

layout(location = 0) out vec4 colorOut;
layout(location = 1) out uint idOut;

void main()
{
	vec3 color;
	vec3 amb;

	vec3  lightDir  = normalize(vec3(1.0, 1.0, 1.0));
	vec3  n         = normalize(normal);
	float intensity = max(dot(lightDir, n), 0.0);

	if(texCount == 0)
	{
		color = diffuse.xyz;
		amb   = ambient.xyz;
	}
	else
	{
		color = texture(texUnit, texCoord).xyz;
		amb   = color.xyz * 0.33;
	}

	vec4 id;

	uint pID = gl_PrimitiveID;

//	id.r = (float((uint(gl_PrimitiveID) >>  0u) & 0xFFu)) / 255.0f;
//	id.g = (float((uint(gl_PrimitiveID) >>  8u) & 0xFFu)) / 255.0f;
//	id.b = (float((uint(gl_PrimitiveID) >> 16u) & 0xFFu)) / 255.0f;
//	id.a = (float((uint(gl_PrimitiveID) >> 24u) & 0xFFu)) / 255.0f;


	id.r = (float((uint(gl_PrimitiveID) >>  0u) & 0xFFu)) / 255.0f;
	id.g = (float((uint(gl_PrimitiveID) >>  8u) & 0xFFu)) / 255.0f;
	id.b = (float((uint(gl_PrimitiveID) >> 16u) & 0xFFu)) / 255.0f;
	id.a = (float((uint(gl_PrimitiveID) >> 24u) & 0xFFu)) / 1.0f;

	colorOut.xyz = (color * intensity) + amb;
	colorOut.a   = 1.0;
	idOut        = gl_PrimitiveID;


//	colorOut = id;


}

