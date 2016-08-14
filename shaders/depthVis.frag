#version 410

in vec2 texCoord;

uniform sampler2DMS depthTex;
uniform float zNear;
uniform float zFar;

layout (location = 0) out vec4 FragColor;

float LinearizeDepth(in ivec2 iTexCoord)
{
    float d = texelFetch(depthTex, iTexCoord, 0).x;

    float zzFar = zFar / 10.f;

    return (2.0 * zNear) / (zzFar + zNear - d * (zzFar - zNear));

//	return d;
}

void main()
{
	ivec2 texDim    = textureSize(depthTex);
	ivec2 iTexCoord = ivec2(texCoord*texDim);
	float c = LinearizeDepth(iTexCoord);
	FragColor = vec4(c, c, c, 1.0);
}