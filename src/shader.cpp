#include "toy2d/shader.hpp"
#include "toy2d/context.hpp"

namespace toy2d {

std::unique_ptr<Shader> Shader::instance_ = nullptr;

void Shader::Init(const std::string& vertexSource, const std::string& fragSource) {
    instance_.reset(new Shader(vertexSource, fragSource));
}

void Shader::Quit() {
    instance_.reset();
}

Shader::Shader(const std::string& vertexSource, const std::string& fragSource) {
    vk::ShaderModuleCreateInfo createInfo;
    // 不推荐使用 createInfo.setCode()，该函数输入为uint32_t数组，
    // 因为读入二进制文件时通常不使用uint32_t的类型读入
    createInfo.codeSize = vertexSource.size();
    createInfo.pCode = (uint32_t*)vertexSource.data();
    vertexModule = Context::GetInstance().device.createShaderModule(createInfo);

    createInfo.codeSize = fragSource.size();
    createInfo.pCode = (uint32_t*)fragSource.data();
    fragmentModule = Context::GetInstance().device.createShaderModule(createInfo);
}

Shader::~Shader() {
    auto& device = Context::GetInstance().device;
    device.destroyShaderModule(vertexModule);
    device.destroyShaderModule(fragmentModule);
}

}
