#version 450 core
layout(location = 0) in vec3 world_Position;
layout(location = 0) out vec4 out_fragment;

uniform samplerCube u_cubemap;

#define PI 3.14159265358979323846

void main() {
    vec3 N = normalize(world_Position);
    vec3 irradiance = vec3(0.0);

    // irradiance
    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = cross(N, right);

    int nSamples = 0;
    float sampleDelta = 0.025;

    for (float theta = 0.0; theta < PI / 2; theta += sampleDelta) {
        for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;
            irradiance += texture(u_cubemap, sampleVec).rgb * cos(theta) * sin(theta);
            nSamples++;
        }
    }

    irradiance = irradiance * sampleDelta * sampleDelta;
    out_fragment = vec4(irradiance, 1.0);
}
