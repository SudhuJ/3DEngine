#version 450 core
layout(location = 0) in vec2 v_UVs;

layout(location = 0) out vec4 out_fragment;

// Standard Gaussian weights for a 9-tap blur (1 center + 4 pixels in each direction)
const float WEIGHTS[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform sampler2D u_brightnessMap;
uniform bool u_horizontalPass;
uniform int u_frameWidth;
uniform int u_frameHeight;

void main() {
    vec3 color = texture(u_brightnessMap, v_UVs).rgb * WEIGHTS[0];
    vec2 texelSize = 1.0 / vec2(u_frameWidth, u_frameHeight);

    if (u_horizontalPass) {
        for (int i = 1; i < 5; ++i) {
            color += texture(u_brightnessMap, v_UVs + vec2(texelSize.x * i, 0.0)).rgb * WEIGHTS[i];
            color += texture(u_brightnessMap, v_UVs - vec2(texelSize.x * i, 0.0)).rgb * WEIGHTS[i];
        }
    }
    else {
        for (int i = 1; i < 5; ++i) {
            color += texture(u_brightnessMap, v_UVs + vec2(0.0, texelSize.y * i)).rgb * WEIGHTS[i];
            color += texture(u_brightnessMap, v_UVs - vec2(0.0, texelSize.y * i)).rgb * WEIGHTS[i];
        }
    }
    out_fragment = vec4(color, 1.0);
}
