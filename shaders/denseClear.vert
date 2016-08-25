//*****************************************************************************
//denseClear.vert**************************************************************
//*****************************************************************************

#version 420

flat out int instanceID;

void main()
{
	float x = -1.0 + float((gl_VertexID & 1) << 2);
	float y = -1.0 + float((gl_VertexID & 2) << 1);
	instanceID  = gl_InstanceID;
	gl_Position = vec4(x, y, 0, 1);
}