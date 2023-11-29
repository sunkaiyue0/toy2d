#include "toy2d/context.hpp"
#include <vector>

namespace toy2d {

std::unique_ptr<Context> Context::instance_ = nullptr;

void Context::Init() {
    instance_.reset(new Context);
}

void Context::Quit() {
    instance_.reset();
}

// 构造函数
Context::Context() {
    // createInfo是一个参数打包的结构体
    vk::InstanceCreateInfo createInfo;  
    // 使用set函数返回对象本身，可以使用列式调用
    // laryer 通过嵌入Vulkan辅助调试
    // Extension 用于扩展功能，比如RTX卡可以拓展RayTracing

    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_VERSION_1_3);  // 推荐设置当前使用Vulkan的版本 VK_API_VERSION_1_3
    createInfo.setPApplicationInfo(&appInfo);

    // 调试时，一定要开验证层，来辅助检查。
    // 如果忘记了层名字，可以使用enumerateInstanceLayerProperties遍历所有vk支持的层。
    // auto layers = vk::enumerateInstanceLayerProperties();
    // for(auto& layer: layers){
    //     std::cout<<layer.layerName << std::endl;
    // }
    
    // 找到名字后设置添加验证层
    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPEnabledLayerNames(layers);

    // 创建Vulkan实例
    instance = vk::createInstance(createInfo);
}

Context::~Context() {
    instance.destroy(); 
}

}
