#version 450 core
layout(location = 0) in vec3 world_Position;
layout(location = 0) out vec4 out_fragment;

uniform sampler2D u_map;

const float INV_2PI = 0.15915494309189534;
const float INV_PI = 0.3183098861837907;

// Equirectangular Projection
vec2 getSphericalUVs(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= vec2(INV_2PI, INV_PI);
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = getSphericalUVs(world_Position);
    vec3 color = texture(u_map, uv).rgb;
    out_fragment = vec4(color, 1.0);
}
