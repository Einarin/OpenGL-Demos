#version 300 es

in vec3 in_Position;

uniform mat4 in_modelViewMatrix;
uniform mat4 in_projectionMatrix;

void main(void){
    gl_Position = in_projectionMatrix * in_modelViewMatrix * vec4(in_Position,1.0);
}