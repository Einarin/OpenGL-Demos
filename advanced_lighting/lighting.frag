#version 330 core

in vec3 vs_WorldNormal;
in vec3 vs_EyeVector;
in vec3 vs_LightVector;
in vec3 vs_WorldPosition;
in float vs_LightDistance;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraWorldPosition;
uniform vec4 materialData;

out vec4 fragColor;

const float pi = 3.14159265;

//specular

//Implements the Microfacet BRDF
//          F(1,h)*G(l,v,h)*D(h)
// f(1,v) = --------------------
//           4*dot(n,l)*dot(n,v)

//The half vector is the angle of surfaces that reflect
//  the light source at the view angle
//  h in Microfacet model
vec3 halfVector(vec3 light, vec3 normal){
    return normalize(light + normal);
}

//Schlick Approximation of Fresnel
// F(l,h) in Microfacet model
vec3 fresnelSchlick(vec3 specularRGB, vec3 light, vec3 halfVec){
    return specularRGB + (1.0-specularRGB)*pow(1.0-dot(light,halfVec),5);
}

// D(h) is the microfacet Normal Distribution
//  generally gaussian-like
//  uses roughness parameter low roughness means most facets close to n
float microfacetDistribution(vec3 normal, vec3 halfVec, float roughness){
    return ((roughness + 2)/(2*pi)) * pow(max(dot(normal,halfVec),0.0),roughness);
}

// G(1,v,h) chance that a microfacet of the given orientation
//  is shadowed and/or masked
//  usually driven by roughness parameter
float simpleGeometry(){
//super simple to start
    return 1;
}
float kelemenSzirmayKalosGeometry(vec3 lightDir,vec3 halfVec){
    float denom = dot(lightDir,halfVec);
    return 1.f/(denom*denom);
}

//mapping to provide good results for roughness
// roughIn is 0-1 value from texutre, maxRough is largest value
float calcRoughness(float maxRough,float roughIn){
    return pow(maxRough,roughIn);
}

vec3 lighting(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 specularRGB, float roughness){
    vec3 halfVec = halfVector(lightDir,viewDir);
    float microfacet = microfacetDistribution(normal,halfVec,roughness);
    vec3 fresnel = fresnelSchlick(specularRGB,lightDir,halfVec);
    float geometry = kelemenSzirmayKalosGeometry(lightDir,halfVec);
    return 0.25 * geometry * microfacet * fresnel * max(dot(normal,lightDir),0.0);
}

//diffuse
// punctual light reflection equation
//  L0(v) = pi*f(lc,v) * lightRadiance(dot(n,lightCenter))

void main(void){
    //need to renormalize since components are linearly interpolated separately
    vec3 normal = normalize(vs_WorldNormal);
    vec3 lightdiff = lightPosition - vs_WorldPosition;
    vec3 lightDirection = normalize(lightdiff);
    float lightDistance = length(lightdiff);
    vec3 eyeDirection = normalize(cameraWorldPosition - vs_WorldPosition);
    //calculate lambertian diffuse term
    vec3 diffuse = materialData.rgb * max(dot(lightDirection,normal), 0.0);
    diffuse = diffuse / materialData.a;
    //calculate blinn-phong specular term
    //vec3 halfVec = normalize(normal + lightDirection);
    //float specularAngle = max(dot(halfVec,normal),0.0);
    //float specular = pow(specularAngle, 16.0); //constant controls the roughness of the material
    //light falls off with the square of the distance
    //float falloff = lightDistance * lightDistance;
    //vec3 lighting = lightColor * (specular + diffuse) / falloff;
    vec3 lighting = lightColor * (diffuse + lighting(lightDirection,normal,eyeDirection,materialData.rgb,materialData.a));
    //Use HDR with simple tonmapping
    //This needs to be done after lighting is accumulated,
    //  because light adds linearly in real life
    vec3 toneMapped = lighting/(lighting+1.0);
    //write our output
    fragColor = vec4(toneMapped,1.0);
}