#pragma once

#include "vulkan/vulkan.hpp"
#include <memory>
#include <cassert>
#include <iostream>
#include "tool.hpp"

namespace toy2d {

// final关键词代表该类不被继承
class Context final {
public:
    static void Init();
    static void Quit();

    static Context& GetInstance() {
        assert(instance_);
        return *instance_;
    }

    ~Context();

    vk::Instance instance;

private:
    // 单例使用智能指针
    static std::unique_ptr<Context> instance_;

    Context();
};

}
