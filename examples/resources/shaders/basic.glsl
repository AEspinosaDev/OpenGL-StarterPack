#stage vertex
#version 460 core

layout(location = 0) in vec3 position;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
}

#stage fragment
#version 460 core

uniform vec3 u_color;

out vec4 FragColor;

void main() {
    FragColor = vec4(u_color, 1.0f);

}