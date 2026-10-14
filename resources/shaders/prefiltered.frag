#version 450 core
layout(location = 0) in vec3 world_Position;
layout(location = 0) out vec3 out_fragment;

const float PI = 3.14159265358979323846;

uniform float u_roughness;
uniform samplerCube u_cubemap;

float RadicalInverseVdC(uint bits) {
    // reverse bit sequence
    return float(bitfieldReverse(bits)) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverseVdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, float roughness, vec3 N) {
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float DistributionGGX(float NdotH, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

void main() {
    vec3 N = normalize(world_Position);
    vec3 R = N;
    vec3 V = R;
    vec3 result = vec3(0.0);

    const uint SAMPLE_COUNT = 256u;
    float totalWeight = 0.0;

    // solid angle of one source texel at mip 0
    float resolution = float(textureSize(u_cubemap, 0).x);
    float saTexel = 4.0 * PI / (6.0 * resolution * resolution);

    for (uint i = 0u; i < SAMPLE_COUNT; i++) {
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, u_roughness, N);
        vec3 L = normalize(2.0 * dot(H, V) * H - V);
        float NdotL = max(dot(N, L), 0.0);

        if (NdotL > 0.0) {
            // filtered importance sampling (Karis): each sample reads a mip covering
            // its share of the lobe, so bright pixels can't produce speckles
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = DistributionGGX(NdotH, u_roughness) * NdotH / (4.0 * HdotV) + 0.0001;
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
            float mip = (u_roughness == 0.0) ? 0.0 : 0.5 * log2(saSample / saTexel);

            result += textureLod(u_cubemap, L, mip).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    result /= totalWeight;
    out_fragment = result;
}
