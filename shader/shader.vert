#version 450

// glsl语法

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    // 与c++不一样的地方在于，main函数返回为空，gl_Position是内置变量，用来存储顶点着色器的输出
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);    // 齐次坐标
}
