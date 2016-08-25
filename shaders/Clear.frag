#version 420

//layout (location = 0) out vec4 FragColor;

layout(binding = 0) writeonly uniform image2D image0;
layout(binding = 1) writeonly uniform image2D image1;
layout(binding = 2) writeonly uniform image2D image2;
layout(binding = 3) writeonly uniform image2D image3;

void main()
{
//	vec4 clear = vec4(0,0,0,0);
	vec4 clear = vec4(.5,.5,.5,1);
	imageStore(image0, ivec2(gl_FragCoord.xy), clear);
	imageStore(image1, ivec2(gl_FragCoord.xy), clear);
	imageStore(image2, ivec2(gl_FragCoord.xy), clear);
	imageStore(image3, ivec2(gl_FragCoord.xy), vec4(0));

	discard;
}

