#pragma once

#include "vulkan/vulkan.hpp"

namespace toy2d {

class Shader final {
public:
    static void Init(const std::string& vertexSource, const std::string& fragSource);
    static void Quit();

    static Shader& GetInstance() {
        return *instance_;
    }

    vk::ShaderModule vertexModule;  // 顶点着色器
    vk::ShaderModule fragmentModule;    // 片段着色器

    ~Shader();

private:
    static std::unique_ptr<Shader> instance_;

    Shader(const std::string& vertexSource, const std::string& fragSource);
};

}
