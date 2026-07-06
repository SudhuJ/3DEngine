#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UVs;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_UVs;
layout(location = 3) out mat3 v_TBNs;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(u_model)));
    v_Position = (u_model * vec4(a_Position, 1.0)).xyz;
    v_Normal = normalMatrix * a_Normal;
    v_TBNs = normalMatrix * mat3(a_Tangent, a_Bitangent, a_Normal);
    v_UVs = a_UVs;

    gl_Position = u_proj * u_view * u_model * vec4(a_Position, 1.0);
}
