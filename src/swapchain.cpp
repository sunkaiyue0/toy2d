#include "toy2d/swapchain.hpp"
#include "toy2d/context.hpp"

namespace toy2d {

Swapchain::Swapchain(int w, int h) {
    queryInfo(w, h);

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.setClipped(true) // 如果GPU的图大于窗口则进行裁剪
              .setImageArrayLayers(1)   // 对于立体视觉渲染，可以使用图像数组层数来表示不同视角下的图像数据，如2层来分别存储左眼和右眼视角图片数组
              .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
              .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)    // 图像和窗口的融合效果为不融合
              .setSurface(Context::GetInstance().surface)   //  
              .setImageColorSpace(info.format.colorSpace)   // 色彩空间
              .setImageFormat(info.format.format)   // 图像格式
              .setImageExtent(info.imageExtent)     // 图像尺寸
              .setMinImageCount(info.imageCount)    // 图像数量
              .setPreTransform(info.transform)      // 图像变换
              .setPresentMode(info.present);        // 呈现模式

    // 一个图像可能同时被多个命令队列使用，此时需要设置图像的共享模式

    auto& queueIndicecs = Context::GetInstance().queueFamilyIndices;    // 这些索引用于指定交换链图像在哪些队列家族之间共享
    // 如果图形操作和呈现操作在同一个队列中，则交换链图像的共享模式是独占模式
    if (queueIndicecs.graphicsQueue.value() == queueIndicecs.presentQueue.value()) {
        createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
    } else {    // 它们在不同的队列家族中，设置图像共享模式为并行模式，以允许交换链图像在多个队列家族之间共享。
        std::array indices = {queueIndicecs.graphicsQueue.value(), queueIndicecs.presentQueue.value()};
        createInfo.setQueueFamilyIndices(indices)
                  .setImageSharingMode(vk::SharingMode::eConcurrent);   // 并行模式
    }
    // 创建交换链
    swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);
}

Swapchain::~Swapchain() {
    // 销毁交换链
    Context::GetInstance().device.destroySwapchainKHR(swapchain);
}

void Swapchain::queryInfo(int w, int h) {
    auto& phyDevice = Context::GetInstance().phyDevice;
    auto& surface = Context::GetInstance().surface;
    // 颜色空间和格式
    auto formats = phyDevice.getSurfaceFormatsKHR(surface);
    info.format = formats[0];
    for (const auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Sint &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {   // 非线性RGB
            info.format = format;
            break;
        }
    }

    auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
    // 同时绘制数量，区间内取2
    info.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);
    // 绘制大小，区间内取窗口大小
    info.imageExtent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.minImageExtent.width);
    info.imageExtent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.minImageExtent.height);
    // 图像变换
    info.transform = capabilities.currentTransform;

    // 呈现模式
    // VK_PRESENT_MODE_IMMEDIATE_KHR：立即呈现模式。图像立即被显示，性能最优，可能会导致撕裂现象（tearing）。
    // VK_PRESENT_MODE_FIFO_KHR：先进先出模式。图像在垂直同步时被显示，防止撕裂现象，但可能引入一定的延迟。
    // VK_PRESENT_MODE_FIFO_RELAXED_KHR：与 FIFO 类似，但允许在 VSync 期间丢弃某些帧以提高性能,也就是在最大绘制数量内不会阻塞。
    // VK_PRESENT_MODE_MAILBOX_KHR：邮箱模式。当交换链为空时呈现图像，可以最大程度地减少延迟，但可能会引入丢帧。
    // 类似于中间件来存放图像，窗口每次完成绘制从中间件取出最新的图像绘制。
    auto presents = phyDevice.getSurfacePresentModesKHR(surface);
    info.present = vk::PresentModeKHR::eFifo;
    for (const auto& present : presents) {
        if (present == vk::PresentModeKHR::eMailbox) {
            info.present = present;
            break;
        }
    }

}

}
