#version 410

layout(location = 0) in vec3 Position;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{

//	// Object Space
//	Position = (modelMat * vec4(Position.xyz, 1.0)).xyz;
//	//World Space
//	Position = (viewMat * vec4(Position.xyz, 1.0)).xyz;
//	//Eye Space
//	Position = (projMat * vec4(Position.xyz, 1.0)).xyz;
//	//Clip Space i.e. Homogeneous clip space


	mat4 viewProjMat = projMat * viewMat;
//	gl_Position = viewProjMat * modelMat * Position;
	gl_Position = viewProjMat * modelMat * vec4(Position.xyz, 1.0);

//	gl_Position =  vec4(Position.xyz, 1.0);

	// w-divide performed automatically
	// NDC space
}
