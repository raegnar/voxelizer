#version 410

out vec4 vertexPos;
out vec2 texCoord;

void main()
{
	float x = -1.0 + float((gl_VertexID & 1) << 2);
	float y = -1.0 + float((gl_VertexID & 2) << 1);
	vertexPos = vec4(x, y, 0, 1);
	x = (x + 1.0f) * 0.5f;
	y = (y + 1.0f) * 0.5f;
	texCoord = vec2(x, y);
	gl_Position = vertexPos;
}