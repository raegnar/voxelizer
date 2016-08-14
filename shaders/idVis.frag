#version 410

in vec2 texCoord;

uniform usampler2D idTex;

layout (location = 0) out vec4 FragColor;

void main()
{
	vec4 id = texture(idTex, texCoord);

//	if(id.x > 10)
//		FragColor = vec4(1,0,0,1);
//	else
		FragColor = id;

//	FragColor.xy = texCoord;
}