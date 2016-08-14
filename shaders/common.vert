#version 410

layout(location = 0) in vec3 Position;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{
    // Do not perform projection here : it is done in the geometry shader,
    // once all computation in world space are done :
    gl_Position = viewMat * modelMat * vec4(Position, 1);
}
