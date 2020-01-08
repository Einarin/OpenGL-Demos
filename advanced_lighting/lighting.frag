#version 300 es

precision highp float;

in vec3 vs_WorldNormal;
in vec3 vs_EyeVector;
in vec3 vs_LightVector;
in vec3 vs_WorldPosition;
in float vs_LightDistance;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cameraWorldPosition;
uniform vec3 materialColor;
uniform float roughness;
uniform float metalness;

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
    return specularRGB + (1.0-specularRGB)*pow(1.0-dot(light,halfVec),5.0);
}

// D(h) is the microfacet Normal Distribution
//  generally gaussian-like
//  uses roughness parameter low roughness means most facets close to n
float microfacetDistribution(vec3 normal, vec3 halfVec, float roughness){
    return ((roughness + 2.0)/(2.0*pi)) * pow(max(dot(normal,halfVec),0.0),roughness);
}
float ggxDistribution(vec3 normal, vec3 halfVec, float roughness){
    float alpha = roughness * roughness;
    float a2 = alpha * alpha;
    float dnh = dot(normal,halfVec);
    float denom = dnh * dnh * (a2 - 1.0)+1.0;
    return a2 / (denom * denom);
}

// G(1,v,h) chance that a microfacet of the given orientation
//  is shadowed and/or masked
//  usually driven by roughness parameter
float simpleGeometry(){
//super simple to start
    return 1.0;
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

vec3 specular(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 specularRGB, float roughness){
    vec3 halfVec = halfVector(lightDir,viewDir);
    float microfacet = microfacetDistribution(normal,halfVec,roughness);
    vec3 fresnel = fresnelSchlick(specularRGB,lightDir,halfVec);
    float geometry = kelemenSzirmayKalosGeometry(lightDir,halfVec);
    return 0.25 * geometry * microfacet * fresnel;
}

//diffuse
// punctual light reflection equation
//  L0(v) = pi*f(lc,v) * lightRadiance(dot(n,lightCenter))

//light falls off with the square of the distance
float lightFalloff(float lightDistance, float lightRadius){
    float lightNumerator = min(1.0 - pow(lightDistance/lightRadius,4.0),1.0);
    return (lightNumerator * lightNumerator)/((lightDistance*lightDistance)+1.0);
}

void main(void){
    //need to renormalize since components are linearly interpolated separately
    vec3 normal = normalize(vs_WorldNormal);
    vec3 lightdiff = lightPosition - vs_WorldPosition;
    vec3 lightDirection = normalize(lightdiff);
    float lightDistance = length(lightdiff);
    vec3 eyeDirection = normalize(cameraWorldPosition - vs_WorldPosition);
    //calculate lambertian diffuse term, metals have no diffuse
    vec3 diffuse = mix(materialColor.rgb,vec3(0.0),metalness);
    //calculate specular color, 0.004 is a good enough specular for all dielectrics
    vec3 specColor = mix(vec3(0.04),materialColor.rgb,metalness);
    float mappedRough = calcRoughness(100000.0,roughness);
    //diffuse = diffuse / materialData.a;
    //calculate blinn-phong specular term
    //vec3 halfVec = normalize(normal + lightDirection);
    //float specularAngle = max(dot(halfVec,normal),0.0);
    //float specular = pow(specularAngle, 16.0); //constant controls the roughness of the material
    
    float falloff = lightFalloff(lightDistance,1000.0);
    //vec3 lighting = lightColor * (specular + diffuse) / falloff;
    vec3 lighting = falloff * lightColor * max(dot(normal,lightDirection),0.0) * (diffuse + specular(lightDirection,normal,eyeDirection,specColor,mappedRough));
    //add some ambient lighting by approximating a light shining straight down
    vec3 skyDir = vec3(0.0,1.0,0.0);
    lighting += lightColor * 0.001 * max(dot(normal,skyDir),0.0) * (diffuse + specular(skyDir,skyDir,skyDir,specColor,mappedRough));
    //Use HDR with simple tonmapping
    //This needs to be done after lighting is accumulated,
    //  because light adds linearly in real life
    vec3 toneMapped = lighting/(lighting+1.0);
    //write our output
    fragColor = vec4(toneMapped,1.0);
}