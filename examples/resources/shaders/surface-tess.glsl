#stage vertex
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 3) in vec2 uv;

out vec2 v_uv0;
out vec2 v_uv1;

uniform vec2 u_tile0; //For the height map
uniform vec2 u_tile1; //For the albedo map

void main() {

    gl_Position = vec4(position, 1.0);

    v_uv0 = uv*u_tile0;
    v_uv1 = uv*u_tile1;
}

#stage control
#version 460 core

//Number of vertices per patch
layout(vertices=4) out;

layout (binding = 0) uniform Camera
{
    mat4 viewProj;
    mat4 view;

}u_camera;


uniform mat4 u_model;
uniform float u_maxResolution;
uniform float u_minResolution;
uniform float u_maxDistance;
uniform float u_minDistance;
uniform bool u_useDynamicRes;


in vec2 v_uv0[];
in vec2 v_uv1[];
out vec2 tc_uv0[];
out vec2 tc_uv1[];
out vec2 tc_step[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tc_uv0[gl_InvocationID] = v_uv0[gl_InvocationID];
    tc_uv1[gl_InvocationID] = v_uv1[gl_InvocationID];

    if(gl_InvocationID == 0)
    {
        const int MAX_LEVEL = int(u_maxResolution);
        //LOD
        if(u_useDynamicRes){
            const int MIN_LEVEL = int(u_minResolution);
            const float MIN_DISTANCE = u_minDistance;
            const float MAX_DISTANCE = u_maxDistance;

            vec4 vPos0 = u_camera.view * u_model * gl_in[0].gl_Position;
            vec4 vPos1 = u_camera.view * u_model * gl_in[1].gl_Position;
            vec4 vPos2 = u_camera.view * u_model * gl_in[2].gl_Position;
            vec4 vPos3 = u_camera.view * u_model * gl_in[3].gl_Position;

            //Normalized distance
            float dist00 = clamp((abs(length(vPos0.xyz))-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            float dist01 = clamp((abs(length(vPos1.xyz))-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            float dist10 = clamp((abs(length(vPos2.xyz))-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            float dist11 = clamp((abs(length(vPos3.xyz))-MIN_DISTANCE) / (MAX_DISTANCE-MIN_DISTANCE), 0.0, 1.0);
            //Lerp between levels
            float tessLevel0 = mix( MAX_LEVEL, MIN_LEVEL, min(dist10, dist00) );
            float tessLevel1 = mix( MAX_LEVEL, MIN_LEVEL, min(dist00, dist01) );
            float tessLevel2 = mix( MAX_LEVEL, MIN_LEVEL, min(dist01, dist11) );
            float tessLevel3 = mix( MAX_LEVEL, MIN_LEVEL, min(dist11, dist10) );

            gl_TessLevelOuter[0] = tessLevel0;
            gl_TessLevelOuter[1] = tessLevel1;
            gl_TessLevelOuter[2] = tessLevel2;
            gl_TessLevelOuter[3] = tessLevel3;

            gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
            gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
        }else{
            gl_TessLevelOuter[0] = MAX_LEVEL;
            gl_TessLevelOuter[1] = MAX_LEVEL;
            gl_TessLevelOuter[2] = MAX_LEVEL;
            gl_TessLevelOuter[3] = MAX_LEVEL;

            gl_TessLevelInner[0] = MAX_LEVEL;
            gl_TessLevelInner[1] = MAX_LEVEL;


        }

        tc_step[gl_InvocationID] = vec2(1/gl_TessLevelInner[0],1/gl_TessLevelInner[1]);

    }
}

#stage eval
#version 460 core

layout (quads, equal_spacing, ccw) in;

layout (binding = 0) uniform Camera
{
    mat4 viewProj;
    mat4 view;
}u_camera;

uniform mat4 u_model;
uniform float u_height;
uniform sampler2D u_heightMap; 

in vec2 tc_uv0[];
in vec2 tc_uv1[];
in vec2 tc_step[];

out vec3 te_pos;
out vec3 te_normal;
out vec2 te_uv0;
out vec2 te_uv1;
out float te_height;

vec2 bilerp2(vec2 p0, vec2 p1, vec2 p2, vec2 p3, float x, float y){
    vec2 t0 = (p1 - p0) * x + p0;
    vec2 t1 = (p3 - p2) * x + p2;
    return (t1 - t0) * y + t0;
}


vec4 bilerp4(vec4 p0, vec4 p1, vec4 p2, vec4 p3, float x, float y){
    vec4 t0 = (p1 - p0) * x + p0;
    vec4 t1 = (p3 - p2) * x + p2;
    return (t1 - t0) * y + t0;
}
vec3 computeShadingNormal(vec4 p0, vec4 p1, vec4 p2, vec4 p3, float x, float y, vec4 pos ,vec4 normal, float scale){
    
    //Get neighbours positions
    vec4 up = bilerp4(p0,p1,p2,p3,x + tc_step[0].x,y);
    vec2 upUV = bilerp2(tc_uv0[0],tc_uv0[1],tc_uv0[2],tc_uv0[3],x + tc_step[0].x,y);
    up += normal * texture(u_heightMap, upUV).r *scale *0.1;

    vec4 down = bilerp4(p0,p1,p2,p3,x - tc_step[0].x,y);
    vec2 downUV = bilerp2(tc_uv0[0],tc_uv0[1],tc_uv0[2],tc_uv0[3],x - tc_step[0].x,y);
    down += normal * texture(u_heightMap, downUV).r *scale *0.1;

    vec4 left = bilerp4(p0,p1,p2,p3,x,y-tc_step[0].y);
    vec2 leftUV = bilerp2(tc_uv0[0],tc_uv0[1],tc_uv0[2],tc_uv0[3],x,y-tc_step[0].y);
    left += normal * texture(u_heightMap, leftUV).r *scale *0.1;

    vec4 right = bilerp4(p0,p1,p2,p3,x,y+tc_step[0].y);
    vec2 rightUV = bilerp2(tc_uv0[0],tc_uv0[1],tc_uv0[2],tc_uv0[3],x,y+tc_step[0].y);
    right += normal * texture(u_heightMap, rightUV).r *scale *0.1;

    return normalize(cross((up-pos).xyz-(down-pos).xyz,(left-pos).xyz-(right-pos).xyz));

}

void main()
{
    //patch coordinate
    float x = gl_TessCoord.x;
    float y = gl_TessCoord.y;

    vec2 newUV0 = bilerp2(tc_uv0[0],tc_uv0[1],tc_uv0[2],tc_uv0[3],x,y);
    vec2 newUV1 = bilerp2(tc_uv1[0],tc_uv1[1],tc_uv1[2],tc_uv1[3],x,y);
    te_height = texture(u_heightMap, newUV0).r;
    te_uv0 = newUV0;
    te_uv1 = newUV1;

    // retrieve control point position coordinates
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;
    vec4 p3 = gl_in[3].gl_Position;

    // compute patch surface normal
    vec4 uVec = p1 - p0;
    vec4 vVec = p2 - p0;
    vec4 normal = normalize( vec4(cross( vVec.xyz,uVec.xyz), 0) );

    vec4 newPos = bilerp4(p0,p1,p2,p3,x,y);
    //Offset point
    const float HEIGHT_SCALE = u_height;
    newPos += normal * te_height *HEIGHT_SCALE *0.1;

    mat4 modelView = u_camera.view * u_model;
    te_pos = (modelView * newPos).xyz;

    vec3 shnormal = computeShadingNormal(p0,p1,p2,p3,x,y,newPos,normal,HEIGHT_SCALE);
    te_normal =  normalize(mat3(transpose(inverse(modelView))) * shnormal);


    gl_Position = u_camera.viewProj * u_model * newPos;
}


#stage fragment
#version 460 core


in vec3 te_pos;
in vec3 te_normal;
in vec2 te_uv0;
in vec2 te_uv1;
in float te_height;

layout (binding = 1) uniform Scene
{
    vec4 ambient;
    vec4 lightPos;
    vec4 lightColor;
}u_scene;

uniform vec3 u_albedo;
uniform float u_roughness;
uniform float u_metalness;
uniform float u_opacity;

uniform bool u_useAlbedoT;
uniform sampler2D u_albedoT;

out vec4 FragColor;

//Surface global properties
vec3 g_normal = te_normal;
vec3 g_albedo = u_useAlbedoT ? texture(u_albedoT,te_uv1).rgb : u_albedo; 
float g_opacity = u_opacity;
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
    vec3 lightDir = normalize(u_scene.lightPos.xyz - te_pos);
    vec3 viewDir = normalize(-te_pos);
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