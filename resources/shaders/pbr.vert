#version 450 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UVs;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in ivec4 a_Joints;
layout(location = 6) in vec4 a_Weights;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_UVs;
layout(location = 3) out mat3 v_TBNs;

uniform mat4 u_model;
uniform mat4 u_proj;
uniform mat4 u_view;

#define MAX_WEIGHTS 4
#define MAX_JOINTS 100

uniform mat4 u_joints[MAX_JOINTS];
uniform bool u_hasJoints = false;

void main() {
    mat4 skinTransform = mat4(1.0);
    if (u_hasJoints) {
        skinTransform = mat4(0.0);
        for (int i = 0; i < MAX_WEIGHTS && a_Joints[i] > -1; i++) {
            skinTransform += u_joints[a_Joints[i]] * a_Weights[i];
        }
    }

    mat4 transform = u_model * skinTransform;

    v_UVs = a_UVs;
    v_Position = (transform * vec4(a_Position, 1.0)).xyz;

    mat3 nMat = mat3(transform);
    v_Normal = nMat * a_Normal;
    v_TBNs = nMat * mat3(a_Tangent, a_Bitangent, a_Normal);

    gl_Position = u_proj * u_view * transform * vec4(a_Position, 1.0);
}
