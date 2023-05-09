#shader vertex
#version 450 core

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

uniform mat4 uSMProj;


out vec3 vNormal;
out vec4 vWorldPos;
out vec2 vTexCoords;
out vec4 vFragPosLight;

void main()
{
    gl_Position = (uProj * uView * uModel) * aPosition;

    vNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vWorldPos = uModel * aPosition;
    vTexCoords = aTexCoords;
    vFragPosLight = uSMProj * uModel * aPosition;
}

#shader pixel
#version 450 core

in vec3 vNormal;
in vec4 vWorldPos;
in vec2 vTexCoords;

in vec4 vFragPosLight;

uniform vec3 uLightPos;
uniform vec3 uCameraPos;
uniform vec3 uDirectionalLight;
uniform vec3 uModelColor;
layout (binding = 0) uniform sampler2D uTextureDiffuse0;
layout (binding = 1) uniform sampler2D uTextureSpecular0;
layout (binding = 2) uniform sampler2D uTextureNormal0;

// shadow map
layout (binding = 3) uniform sampler2D uSMTexture;

out vec4 fragColor;


// shadow test variables
uniform bool uPcfEnabled;
uniform bool uShadowEnabled;
uniform int uSampleRange;


float ShadowCalc(vec4 fragPosLight)
{
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    projCoords = (projCoords * 0.5) + 0.5;

    float closestDepth = texture(uSMTexture, projCoords.xy).r;
    float currentDepth = projCoords.z;

    //float bias = max(0.005 * (1.0 - dot(normalize(vNormal), normalize(-uDirectionalLight))), 0.005);
    float bias = 0.00;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    // pcf filter
    if(uPcfEnabled)
    {
        vec2 texelSize = 1.0 / textureSize(uSMTexture, 0);

        const int loopRange = (uSampleRange - 1) / 2;

        for(int x = -loopRange; x <= loopRange; x++)
        {
            for(int y = -loopRange; y <= loopRange; y++)
            {
                float pcfDepth = texture(uSMTexture, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= pow(uSampleRange,2);
    }

    return shadow;
}

void main()
{
    // Material Constants
    vec3 materialColor = vec3(1.0, 1.0, 1.0);
    float diffuseStrength = 0.7f;
    float specularStrength = 0.7f;
    float specularPower  = 32;
    float a =  0.0075f, b = 0.045f, c = 1.0f;

    float diffuse, specular, ambient;

    vec3 lightDir = normalize(uLightPos - vWorldPos.xyz);
    vec3 cameraDir = normalize(uCameraPos - vWorldPos.xyz);
    float x = length(uLightPos - vWorldPos.xyz);
    float attentuation = 1 / (a * pow(x,2) + b * x + c);
    vec3 reflectDir = reflect(-lightDir, vNormal);

    // point light
    ambient = 0.5f;
    diffuse = diffuseStrength * attentuation * max(dot(normalize(vNormal), lightDir), 0.0);
    specular = specularStrength * attentuation * pow(max(dot(reflectDir, cameraDir), 0.0), specularPower);

    // directional light
    lightDir = normalize(-uDirectionalLight);
    reflectDir = reflect(-lightDir, vNormal);
    diffuse += diffuseStrength * max(dot(normalize(vNormal), lightDir), 0.0);
    specular += specularStrength * pow(max(dot(reflectDir, cameraDir), 0.0), specularPower);


    vec3 ambientColor =  ambient  * uModelColor;//* texture(uTextureDiffuse0, vTexCoords).rgb;
    vec3 diffuseColor =  diffuse  * uModelColor;//* texture(uTextureDiffuse0, vTexCoords).rgb;
    vec3 specularColor = specular * uModelColor;// * texture(uTextureSpecular0, vTexCoords).rgb;

    float shadow = 0.0f;
    if(uShadowEnabled)
        shadow = ShadowCalc(vFragPosLight);

    vec3 finalColor = ((diffuseColor + specularColor) * (1.0 - shadow)  + ambientColor) * materialColor;
    fragColor = vec4(finalColor, 1.0);

}