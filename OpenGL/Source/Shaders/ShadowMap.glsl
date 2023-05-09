#shader vertex
#version 450 core

layout (location = 0) in vec4 aPosition;

uniform mat4 uSMProj;
uniform mat4 uModel;

void main()
{
    gl_Position = uSMProj * uModel * aPosition;
}

#shader pixel
#version 450 core


void main()
{
}