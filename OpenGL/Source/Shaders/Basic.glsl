#shader vertex
#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 color;

uniform mat4 uMVP;

out vec3 vColor;

void main()
{
    gl_Position = uMVP * position;
    vColor = color;
}

#shader pixel
#version 330 core

uniform vec4 uColor;
in vec3 vColor;

out vec4 fragColor;

void main()
{
    //fragColor = vec4(0.21, 0.83, 0.4, 1.0);
    fragColor = vec4(vColor, 1.0f) * uColor;
}