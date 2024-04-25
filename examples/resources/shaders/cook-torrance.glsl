#stage vertex
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 color;

layout (binding = 0) uniform Camera
{
    mat4 viewProj;
    mat4 view;
}u_camera;

uniform mat4 u_model;
uniform vec2 u_tile;

out vec3 _pos;
out vec3 _modelPos;
out vec3 _normal;
out vec2 _uv;
out vec3 _color;

void main() {
    
    mat4 modelView = u_camera.view * u_model;

    _pos = (modelView * vec4(position, 1.0)).xyz;

    _modelPos = (u_model * vec4(position, 1.0)).xyz;

    _normal = normalize(mat3(transpose(inverse(modelView))) * normal);

    _color = color;

    _uv = uv * u_tile;

    gl_Position = u_camera.viewProj  * u_model * vec4(position, 1.0);


}

#stage fragment
#version 460 core

in vec3 _pos;
in vec3 _modelPos;
in vec3 _normal;
in vec2 _uv;
in vec3 _color;

layout (binding = 1) uniform Scene
{
    vec4 ambient;
    vec4 lightPos;
    vec4 lightColor;
}u_scene;

uniform vec3 u_albedo;
uniform float u_roughness;
uniform float u_metalness;

uniform bool u_useAlbedoT;
uniform sampler2D u_albedoT;

out vec4 FragColor;

//Surface global properties
vec3 g_normal = _normal;
vec3 g_albedo = u_useAlbedoT ? texture(u_albedoT,_uv).rgb : u_albedo; 
float g_opacity = 1.0;
float g_roughness = u_roughness;
float g_metalness = u_metalness;
float g_ao = 1.0;

//Constant
const float PI = 3.14159265359;

///Fresnel
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

//Normal Distribution 
//Trowbridge - Reitz GGX
float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}
//Geometry
//Schlick - GGX
float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 computeLighting() {

    //Vector setup
    vec3 lightDir = normalize(u_scene.lightPos.xyz - _pos);
    vec3 viewDir = normalize(-_pos);
    vec3 halfVector = normalize(lightDir + viewDir); 

	//Heuristic fresnel factor
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, g_albedo, g_metalness);

	//Radiance
    vec3 radiance = u_scene.lightColor.xyz * u_scene.lightPos.w ; //* computeAttenuation(...)


	// Cook-Torrance BRDF
    float NDF = distributionGGX(g_normal, halfVector, g_roughness);
    float G = geometrySmith(g_normal, viewDir, lightDir, g_roughness);
    vec3 F = fresnelSchlick(max(dot(halfVector, viewDir), 0.0), F0);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - g_metalness;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(g_normal, viewDir), 0.0) * max(dot(g_normal, lightDir), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

	// Add to outgoing radiance result
    float lambertian = max(dot(g_normal, lightDir), 0.0);
    return (kD * g_albedo / PI + specular) * radiance * lambertian;

}
float computeFog() {
    float min = 0.5;
    float max = 100.0;
    float z = (2.0 * min) / (max + min - gl_FragCoord.z * (max - min));
    return exp(-u_scene.lightColor.w * 0.01 * z);
}


void main() {
    
    vec3 color = computeLighting();
    
    //Ambient component
    vec3 ambient = (u_scene.ambient.w * 0.1 * u_scene.ambient.xyz) * g_albedo * g_ao;
    color += ambient;

    float f = computeFog();
    color = f * color + (1 - f) * vec3(0.5);

	//Tone Up
    color = color / (color + vec3(1.0));

    //Gamma Correction 
    const float GAMMA = 2.2;
    color = pow(color, vec3(1.0 / GAMMA));

    FragColor = vec4(color, g_opacity);

}