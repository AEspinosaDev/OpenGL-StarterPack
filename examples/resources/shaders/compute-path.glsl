#version 460 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#define MAX_VERTICES 30

layout(std140, binding = 0) buffer PositionBuffer {
        vec4 positions[MAX_VERTICES];
};
layout(std140, binding = 1) buffer ForwardBuffer {
        vec4 forward[MAX_VERTICES];
};
layout(std140, binding = 2) buffer VelocityBuffer {
        vec4 velocity[MAX_VERTICES];
};

uniform float u_time;
uniform float u_speed; 

void main() {
        uint idx = gl_GlobalInvocationID.x;     

        float angle = (u_speed+velocity[idx].x) * 0.001 ;
        
        vec3 v = positions[idx].xyz;
        float newX = v.x * cos(angle) - v.z * sin(angle);
        float newZ = v.x * sin(angle) + v.z * cos(angle);
    
        positions[idx].x =  newX;
        positions[idx].z =  newZ;

        vec3 newForward = cross(v, vec3(0.0,1.0,0.0));
        forward[idx].xyz = normalize(newForward);
}