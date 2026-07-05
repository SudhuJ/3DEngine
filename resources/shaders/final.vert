#version 450 core
layout(location = 0) in vec4 a_quad;
layout(location = 0) out vec2 uvs;

void main() {
    uvs = vec2(a_quad.z, a_quad.w);
    gl_Position = vec4(a_quad.x, a_quad.y, 0.0, 1.0);
}
