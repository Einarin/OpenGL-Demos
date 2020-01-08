#version 300 es

precision highp float;

in vec3 vs_WorldNormal;
in vec3 vs_EyeVector;
in vec3 vs_LightVector;
in vec3 vs_WorldPosition;
in float vs_LightDistance;

uniform vec3 lightPosition;
uniform vec3 lightColor;

out vec4 fragColor;

void main(void){
    //need to renormalize since components are linearly interpolated separately
    vec3 normal = normalize(vs_WorldNormal);
    vec3 lightdiff = lightPosition - vs_WorldPosition;
    vec3 lightDirection = normalize(lightdiff);
    float lightDistance = length(lightdiff);
    //calculate lambertian diffuse term
    float diffuse = max(dot(lightDirection,normal), 0.0);
    //calculate blinn-phong specular term
    vec3 halfVec = normalize(normal + lightDirection);
    float specularAngle = max(dot(halfVec,normal),0.0);
    float specular = pow(specularAngle, 16.0); //constant controls the roughness of the material
    //light falls off with the square of the distance
    float falloff = lightDistance * lightDistance;
    vec3 lighting = lightColor * (specular + diffuse) / falloff;
    vec3 color = min(lighting,vec3(1.0)); //we can't display a light brighter than 1 yet (HDR is the proper fix for this)
    fragColor = vec4(color,1.0);
}