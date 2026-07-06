#version 450 core
layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_UVs;
layout(location = 3) in mat3 v_TBNs;

layout(location = 0) out vec4 out_fragment;
layout(location = 1) out vec4 out_brightness;

const vec3 BLOOM_THRESHOLD = vec3(0.2126, 0.7152, 0.0722); // relative luminance
#define PI 3.14159265358979323846
#define MAX_LIGHTS 10

uniform samplerCube u_IrradMap;
uniform samplerCube u_PrefilteredMap;
uniform sampler2D u_DepthMap;
uniform mat4 u_LightSpace;
uniform sampler2D u_BRDFMap;

struct Material {
    vec3 Albedo;
    vec3 Emissive;
    float Roughness;
    float Metallic;
    float Occlusion;

    sampler2D AlbedoMap;
    sampler2D EmissiveMap;
    sampler2D OcclusionMap;
    sampler2D RoughnessMap;
    sampler2D MetallicMap;
    sampler2D NormalMap;

    bool useAlbedoMap;
    bool useEmissiveMap;
    bool useOcclusionMap;
    bool useRoughnessMap;
    bool useMetallicMap;
    bool useNormalMap;
};

struct pointLight {
    float Intensity;
    vec3 Position;
    vec3 Radiance;
};

struct directLight {
    float Intensity;
    vec3 Direction;
    vec3 Radiance;
};

struct spotLight {
    float Intensity;
    vec3 Position;
    vec3 Radiance;
    vec3 Direction;
    float Falloff;
    float Cutoff;
};

uniform Material u_material;
uniform vec3 u_viewPos;
uniform pointLight u_pointLights[MAX_LIGHTS];
uniform directLight u_directLights[MAX_LIGHTS];
uniform spotLight u_spotLights[MAX_LIGHTS];
uniform int u_npointLights;
uniform int u_ndirectLights;
uniform int u_nspotLights;

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (vec3(1.0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 computeAmbientLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) {
    float cosTheta = max(dot(N, V), 0.0);
    vec3 F = fresnelSchlick(cosTheta, F0);
    vec3 kD = mix(vec3(1.0) - F, vec3(0.0), metallic);
    vec3 diffuseIBL = kD * albedo * texture(u_IrradMap, N).rgb;
    vec3 Lr = 2.0 * cosTheta * N - V;
    int mipLevels = textureQueryLevels(u_PrefilteredMap);
    vec3 Ks = textureLod(u_PrefilteredMap, Lr, roughness * float(mipLevels - 1)).rgb;

    vec2 brdf = texture(u_BRDFMap, vec2(cosTheta, roughness)).rg;
    vec3 specularIBL = (F0 * brdf.x + brdf.y) * Ks;

    return diffuseIBL + specularIBL;
}

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float geometrySmith(float NdotV, float NdotL, float roughness) {
    float ggx1 = geometrySchlickGGX(NdotV, roughness);
    float ggx2 = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom);
}

vec3 computePointLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 result = vec3(0.0);
    float NdotV = max(dot(N, V), 0.0);

    for (int i = 0; i < u_npointLights; i++) {
        vec3 L = normalize(u_pointLights[i].Position - v_Position);
        vec3 H = normalize(L + V);

        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance BRDF
        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(NdotV, NdotL, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 diffuse = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 specular = NDF * G * F / max(4.0 * NdotV * NdotL, 0.001);

        float distance = length(u_pointLights[i].Position - v_Position);
        float attenuation = u_pointLights[i].Intensity / (distance * distance);

        result += (diffuse * albedo / PI + specular) * u_pointLights[i].Radiance * attenuation * NdotL;
    }
    return result;
}

vec3 computeDirectLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 result = vec3(0.0);
    float NdotV = max(dot(N, V), 0.0);

    for (int i = 0; i < u_ndirectLights; i++) {
        vec3 L = -normalize(u_directLights[i].Direction);
        vec3 H = normalize(V + L);

        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance BRDF
        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(NdotV, NdotL, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 diffuse = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 specular = NDF * G * F / max(4.0 * NdotV * NdotL, 0.001);

        result += (diffuse * albedo / PI + specular) *
                NdotL * u_directLights[i].Radiance * u_directLights[i].Intensity;
    }
    return result;
}

vec3 computeSpotLight(vec3 N, vec3 V, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 result = vec3(0.0);
    float NdotV = max(dot(N, V), 0.0);

    for (int i = 0; i < u_nspotLights; i++) {
        vec3 L = normalize(u_spotLights[i].Position - v_Position);
        vec3 H = normalize(V + L);

        float NdotL = max(dot(N, L), 0.0);

        // Cook-Torrance BRDF
        float NDF = distributionGGX(N, H, roughness);
        float G = geometrySmith(NdotV, NdotL, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 diffuse = (vec3(1.0) - F) * (1.0 - metallic);
        vec3 specular = NDF * G * F / max(4.0 * NdotV * NdotL, 0.001);

        float theta = dot(L, -normalize(u_spotLights[i].Direction));
        float epsilon = (u_spotLights[i].Cutoff - u_spotLights[i].Falloff);
        float spotFactor = clamp((theta - u_spotLights[i].Falloff) / epsilon, 0.0, 1.0);

        // light attentuation
        float a = 1.0;
        float b = 0.5;
        float distance = length(u_spotLights[i].Position - v_Position);
        float denom = a * distance * distance + b * distance + 1;
        float attenuation = 1.0 / denom;

        result += (diffuse * albedo / PI + specular) *
                u_spotLights[i].Radiance * u_spotLights[i].Intensity * spotFactor *
                attenuation * NdotL;
    }
    return result;
}

float computeShadow() {
    vec4 position = u_LightSpace * vec4(v_Position, 1.0);
    vec3 uvs = (position.xyz / position.w) * 0.5 + 0.5;
    float depth = texture(u_DepthMap, uvs.xy).r;
    return depth < uvs.z - 0.005 ? 1.0 : 0.0;
}

void main() {
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(u_viewPos - v_Position);

    if (u_material.useNormalMap) {
        N = normalize(v_TBNs * (texture(u_material.NormalMap, v_UVs).rgb * 2.0 - 1.0));
    }

    vec3 albedo = u_material.Albedo;
    vec3 emissive = u_material.Emissive;
    float metallic = u_material.Metallic;
    float roughness = u_material.Roughness;
    float occlusion = u_material.Occlusion;

    if (u_material.useAlbedoMap) {
        albedo *= texture(u_material.AlbedoMap, v_UVs).rgb;
    }
    if (u_material.useEmissiveMap) {
        emissive *= texture(u_material.EmissiveMap, v_UVs).rgb;
    }
    if (u_material.useOcclusionMap) {
        occlusion *= texture(u_material.OcclusionMap, v_UVs).r;
    }
    if (u_material.useRoughnessMap) {
        roughness *= texture(u_material.RoughnessMap, v_UVs).r;
    }
    if (u_material.useMetallicMap) {
        metallic *= texture(u_material.MetallicMap, v_UVs).r;
    }

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    vec3 ambient = computeAmbientLight(N, V, F0, albedo, roughness, metallic);

    vec3 direct = computePointLight(N, V, F0, albedo, roughness, metallic);
    direct += computeDirectLight(N, V, F0, albedo, roughness, metallic);
    direct += computeSpotLight(N, V, F0, albedo, roughness, metallic);

    vec3 result = ambient + (1.0 - computeShadow()) * direct;
    result = result * occlusion + emissive;

    if (dot(result, BLOOM_THRESHOLD) > 1.0) {
        out_brightness = vec4(result, 1.0);
    } else {
        out_brightness = vec4(0.0, 0.0, 0.0, 1.0);
    }

    out_fragment = vec4(result, 1.0);
}
