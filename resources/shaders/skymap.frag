#version 450 core
layout(location = 0) in vec3 world_Position;
layout(location = 0) out vec4 out_fragment;

uniform sampler2D u_map;

// Equirectangular Projection
vec2 getSphericalUVs(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= vec2(0.1591, 0.3183);
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = getSphericalUVs(world_Position);
    vec3 color = texture(u_map, uv).rgb;
    out_fragment = vec4(color, 1.0);
}
