
#version 410

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform mat4 warpMatrix;


layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec3 Normal;

void main()
{
	Normal      = normalize(vec3(viewMatrix * modelMatrix * vec4(normal,0.0)));
	TexCoord    = vec2(texCoord);
	vec4 clipVert = projMatrix * viewMatrix * modelMatrix * vec4(position,1.0);

	gl_Position = warpMatrix * clipVert;
}

