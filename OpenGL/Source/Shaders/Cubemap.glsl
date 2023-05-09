#shader vertex
#version 450 core

layout (location = 0) in vec4 aPosition;

uniform mat4 uView;
uniform mat4 uProj;

out vec3 vTexCoords;

void main()
{
    vec4 pos = (uProj * uView) * aPosition;
    gl_Position = pos.xyww;

    vTexCoords = vec3(aPosition.x, aPosition.y, aPosition.z);
}

#shader pixel
#version 450 core

in vec3 vTexCoords;

layout (binding = 0) uniform samplerCube skybox;

out vec4 fragColor;

void main()
{
    fragColor = texture(skybox, vTexCoords);
}