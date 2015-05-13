#version 330 core

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TexCoord;

out vec3 vs_WorldPosition;
out vec3 vs_WorldNormal;
out vec3 vs_EyeVector;
out vec3 vs_LightVector;
out float vs_LightDistance;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraWorldPosition;
uniform vec3 lightPosition;

void main(void){
    mat4 normalMatrix = transpose(inverse(modelMatrix));
    vs_WorldNormal = normalize(normalMatrix * vec4(in_Normal,0.0)).xyz;
    vec4 worldPosition = modelMatrix * vec4(in_Position,1.0);
    vs_WorldPosition = worldPosition.xyz;
    vs_EyeVector = normalize(cameraWorldPosition - worldPosition.xyz);
    vs_LightVector = normalize(lightPosition - worldPosition.xyz);
    vs_LightDistance = length(lightPosition - worldPosition.xyz);
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}