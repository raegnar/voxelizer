
#version 410


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[];
in vec3 Normal[];

out vec2 texCoord;
out vec3 normal;

void main()
{
    gl_Position = gl_in[0].gl_Position; texCoord = TexCoord[0]; normal = Normal[0];	EmitVertex();
	gl_Position = gl_in[1].gl_Position; texCoord = TexCoord[1]; normal = Normal[1];	EmitVertex();
	gl_Position = gl_in[2].gl_Position; texCoord = TexCoord[2]; normal = Normal[2];	EmitVertex();
	gl_PrimitiveID = gl_PrimitiveIDIn;
	EndPrimitive();
}