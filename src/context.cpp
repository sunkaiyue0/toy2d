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

Context::Context() {
    createInstance();       // 创建vk实例
    pickupPhyiscalDevice();     // 挑选物理设备（用于获取物理显卡的信息）
    queryQueueFamilyIndices();      // 查找队列族索引
    createDevice();      // 创建逻辑设备（Vulkan不能直接与物理显卡交互）
    getQueues();        // 获取命令队列
}

Context::~Context() {
    device.destroy();
    instance.destroy(); 
}
// 封装创建vk实例
void Context::createInstance() {
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_VERSION_1_3);
    createInfo.setPApplicationInfo(&appInfo);
    instance = vk::createInstance(createInfo);

    std::vector<const char*> layers = {"VK_LAYER_KHRONOS_validation"};

    RemoveNosupportedElems<const char*, vk::LayerProperties>(layers, vk::enumerateInstanceLayerProperties(),
                           [](const char* e1, const vk::LayerProperties& e2) {
                                return std::strcmp(e1, e2.layerName) == 0;
                           });
    createInfo.setPEnabledLayerNames(layers);

    instance = vk::createInstance(createInfo);
}

void Context::pickupPhyiscalDevice() {
    auto devices = instance.enumeratePhysicalDevices();
    // 是否自信返回?
    // 否
    // for (auto& device : devices){
    //     auto feature = device.getFeatures();
    //     if(feature.geometryShader){
    //         phyDevice = device;
    //         break;
    //     }
    // }
    // 是
    phyDevice = devices[0];
    // .deviceType:
    // vk::PhysicalDeviceType::TYPE;
    std::cout << phyDevice.getProperties().deviceName << std::endl;
}

void Context::createDevice() {
    // 可以为逻辑设备设置 layer 和 extension
    // 如果在Instance设置过，则会顺延
    // 但逻辑设备有一些独有的 layer 和 extension
    vk::DeviceCreateInfo createInfo;

    // Queue的全称为Command Queue，用于物理设备与逻辑设备的交互的桥梁
    // 注意：命令队列嵌入在逻辑设备中，因此会随着逻辑设备销毁而销毁
    // logDevice<-->CommandQueue<-->phyDevice
    // CommandQueue<-CommandBuffer<-CMD
    vk::DeviceQueueCreateInfo queueCreateInfo;  // 优先级队列
    float priorities = 1.0;     // 优先级最高为1，最低为0
    queueCreateInfo.setPQueuePriorities(&priorities)
                   .setQueueCount(1)    // 队列的大小
                   .setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());  // 设置命令队列类型（图像、计算、传输操作等）
    createInfo.setQueueCreateInfos(queueCreateInfo);

    // 通过物理设备对象创建逻辑设备 
    device = phyDevice.createDevice(createInfo);
}

void Context::queryQueueFamilyIndices() {
    auto properties = phyDevice.getQueueFamilyProperties();     // 获得物理设备支持的队列族属性
    for (int i = 0; i < properties.size(); i++) {
        const auto& property = properties[i];
        if (property.queueFlags & vk::QueueFlagBits::eGraphics) {   // 标记graphics队列族索引
            queueFamilyIndices.graphicsQueue = i;
            break;
        }
    }
}

void Context::getQueues() {
    // 通过队列族索引和队列索引 定位队列
    graphcisQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
}

}