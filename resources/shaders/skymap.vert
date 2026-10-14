#version 450 core
// required for separable programs (strictly enforced by NVIDIA)
out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) in vec3 a_Position;
layout(location = 0) out vec3 world_Position;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
    gl_Position = (u_proj * u_view * vec4(a_Position, 1.0));
    world_Position = normalize(a_Position);
}
