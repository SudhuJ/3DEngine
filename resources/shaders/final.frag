#version 450 core
layout(location = 0) out vec4 out_fragment;
layout(location = 0) in vec2 uvs;

uniform sampler2D u_map;

const float GAMMA = 2.2;
const float EXPOSURE = 10;

void main() {
    vec3 result = texture(u_map, uvs).rgb;
    result = vec3(1.0) - exp(-result * EXPOSURE);
    result = pow(result, vec3(1.0 / max(GAMMA, 0.0001)));
    out_fragment = vec4(result, 1.0);
}
