#version 330 core

out vec4 fragColor;

void main(void){
    float normalizedDepth = gl_FragCoord.z*gl_FragCoord.w;
    vec3 color = vec3(normalizedDepth);
    fragColor = vec4(color,1.0);
}