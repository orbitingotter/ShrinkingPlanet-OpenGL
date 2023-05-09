#shader vertex
#version 330 core

layout (location = 0) in vec4 position;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main()
{
    gl_Position = (uProj * uView * uModel) * position;
}

#shader pixel
#version 330 core

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0);
}