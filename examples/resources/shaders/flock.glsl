#stage vertex
#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 up;
layout(location = 2) in vec4 forward;
layout(location = 3) in vec4 varianze;

out vec3 v_up;
out vec3 v_forward;
out float v_velocity;
out float v_colorVarianze;

void main() {
    gl_Position = position;

    v_up =  up.xyz;
    v_forward = forward.xyz;
    v_velocity = varianze.y;
    v_colorVarianze =varianze.z;
}

#stage geometry
#version 460 core

layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

in vec3 v_up[];
in vec3 v_forward[];
in float v_velocity[];
in float v_colorVarianze[];


layout (binding = 0) uniform Camera
{
    mat4 viewProj;
    mat4 view;
}u_camera;

uniform float u_birdSize;
uniform float u_wingLength;
uniform float u_angularSpeed;

uniform float u_time;

uniform mat4 u_model;

out vec2 g_uv;
out int g_isBody;
out float g_colorVarianze;

//Constant
const float PI = 3.14159265359;

void translateAndEmitPoint(vec3 fwdDelta, vec3 upDelta, vec3 rightDelta, vec2 uv, vec3 shNormal){
    //Here we transform and project the actual points
    vec3 newPos = gl_in[0].gl_Position.xyz + fwdDelta + upDelta + rightDelta;
    g_uv = uv;
    g_colorVarianze = v_colorVarianze[0];
    gl_Position =  u_camera.viewProj * u_model * vec4(newPos,1.0);
    EmitVertex();
}

void createBody(){
       float  halfSize = u_birdSize*0.5;
       float  height = u_birdSize*0.25;

       vec3 fwdOffset =  (v_forward[0] * halfSize);
       vec3 upOffset =  (v_up[0] * height);

        vec3 shNormal = cross(v_forward[0],v_up[0]);

        g_isBody = 1; //This vertices represent the body

        translateAndEmitPoint(-fwdOffset,-upOffset,vec3(0.0f),vec2(0,1),shNormal);
        translateAndEmitPoint(+fwdOffset,-upOffset,vec3(0.0f),vec2(1,1),shNormal);
        translateAndEmitPoint(-fwdOffset,+upOffset,vec3(0.0f),vec2(0,0),shNormal);
        translateAndEmitPoint(fwdOffset,upOffset,vec3(0.0f),vec2(1,0),shNormal);

        EndPrimitive();

}

void createWing(bool isRight){

    vec3 right = isRight ? cross(v_forward[0],v_up[0]):cross(v_up[0],v_forward[0]) ;
    
    vec3 fwdOffset =  (v_forward[0] * u_wingLength * 0.5);
    vec3 rightOffset = right;

    //Rotation around forward axis in order to animate the wings
    float angle =  PI/4.0 * sin(u_time*(u_angularSpeed));
    float sinAngle = sin(angle);
    float cosAngle = cos(angle);
    rightOffset.x = rightOffset.x * cosAngle - rightOffset.y * sinAngle;
    rightOffset.y = rightOffset.x * sinAngle + rightOffset.y * cosAngle;
    if(isRight){
     rightOffset.y*=-1.0;
    }   

    g_isBody = 0; //Its not the body anymore!

    //Tricky uv setup
    translateAndEmitPoint(-fwdOffset,vec3(0.0f),vec3(0.0f),vec2(1,0.65),vec3(0.0));
    translateAndEmitPoint(+fwdOffset,vec3(0.0f,0.05f,0.0f),vec3(0.0f),vec2(1,0.35),vec3(0.0));
    translateAndEmitPoint(-fwdOffset,vec3(0.0f,0.2f,0.0f), rightOffset,vec2(0.5,0.7),vec3(0.0));
    translateAndEmitPoint(fwdOffset,vec3(0.0f,0.2f,0.0f), rightOffset,vec2(0.5,0.3),vec3(0.0));
    translateAndEmitPoint(-fwdOffset,vec3(0.0f), rightOffset*2.0,vec2(0,0.7),vec3(0.0));
    translateAndEmitPoint(fwdOffset,vec3(0.0f), rightOffset*2.0,vec2(0,0.3),vec3(0.0));

    EndPrimitive();

}

void createBird(){
    createBody();
    createWing(false);
    createWing(true);
}

void main(){
    createBird();
}


#stage fragment
#version 460 core


in vec2 g_uv;
in flat int g_isBody;
in flat float g_colorVarianze;

uniform sampler2D u_bodyT;
uniform sampler2D u_wingT;

out vec4 FragColor;

const float EPSILON = 0.05;

void main() {
    if(g_isBody==1){
        //If pixel inside the body
        FragColor = texture2D(u_bodyT,g_uv);
    }else{
         //If pixel inside the wings
        FragColor = texture2D(u_wingT,g_uv);
    }
    FragColor.xyz *= g_colorVarianze;
    //Alpha test. To use blending correctly we should need to use OIT methods or primitive level ordering ...
    if(FragColor.a < 1.0-EPSILON) discard;

}