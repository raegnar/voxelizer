#version 410

in vec4 vertexPos;
in vec2 texCoord;

layout (location = 0) out vec4 FragColor;

uniform sampler2D tex;

void main()
{
	vec4 texColor = texture(tex, texCoord);
	FragColor = vec4(texColor.xyz, 1);
}