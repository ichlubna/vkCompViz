module vkCompViz;

import std;

using namespace vkCompViz;

App::App()
{
}

void App::useWindow(Window::Parameters const &windowParameters)
{
    window = std::make_unique<Window::WindowGlfw>(windowParameters);
}

void App::run(ComputeParameters const &computeParameters)
{
    Gpu::Vulkan::VulkanInitParams vulkanInitParams;
    if (window)
        vulkanInitParams.requiredExtensions = window->requiredExtensions();
    gpu = std::make_unique<Gpu::Vulkan>(vulkanInitParams);
   
     if(window)
        window->run();
}

App::~App()
{
}
