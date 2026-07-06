#version 450 core
layout(location = 0) out vec4 out_fragment;
layout(location = 0) in vec2 uvs;

uniform sampler2D u_map;
uniform sampler2D u_bloom;

const float MIN_GAMMA = 0.000001;
const float GAMMA = 2.5;
const float EXPOSURE = 10;

void main() {
    vec3 result = texture(u_map, uvs).rgb + texture(u_bloom, uvs).rgb;
    result = pow(result, vec3(GAMMA));
    result = vec3(1.0) - exp(-result * EXPOSURE);
    result = pow(result, vec3(1.0 / max(GAMMA, MIN_GAMMA)));
    out_fragment = vec4(result, 1.0);
}
