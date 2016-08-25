//*****************************************************************************
//gen3Dcoords.vert*************************************************************
//*****************************************************************************

#version 420

//In the fragment shader ivec3(x,y,instanceID) will provide 
//coordinates for the volume

flat out int instanceID;

void main()
{
	float x = -1.0 + float((gl_VertexID & 1) << 2);
	float y = -1.0 + float((gl_VertexID & 2) << 1);
	instanceID  = gl_InstanceID;
	gl_Position = vec4(x, y, 0, 1);
}