#version 450 core
layout(location = 0) in vec3 world_Position;
layout(location = 0) out vec4 out_fragment;

uniform samplerCube u_map;

void main() {
    out_fragment = vec4(texture(u_map, world_Position).rgb, 1.0);
}
