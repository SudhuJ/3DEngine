#version 450 core
layout(location = 0) in vec4 a_quad;
layout(location = 0) out vec2 v_UVs;

void main() {
    v_Normal = transpose(inverse(mat3(u_model))) * a_Normal;
    v_TBNs = transpose(inverse(mat3(u_model))) * mat3(a_Tangent, a_Bitangent, a_Normal);
}
