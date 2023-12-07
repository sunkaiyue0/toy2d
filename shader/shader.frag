#version 450
// 片段着色器没有内置变量，因此必须由一个out vec4变量来存储颜色RGBA
// location 代表片段着色器中的输入变量在顶点着色器中的索引
// out 指明为输出变量
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
