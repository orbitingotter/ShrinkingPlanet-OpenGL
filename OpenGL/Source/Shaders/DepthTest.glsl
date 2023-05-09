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

float near = 0.1;
float far  = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z) / 20.0;
    fragColor = vec4(vec3(depth), 1.0);
}