module;
#include <memory>
module vkCompViz;
import std;
using namespace vkCompViz;

App::App()
{
}

void App::useWindow(Window::Parameters const &windowParameters)
{
    window = std::make_unique<Window::Glfw>(windowParameters);
}

void App::run(ComputeParameters const &computeParameters)
{
    shader = std::make_unique<Shader::SlangFactory>();
    Gpu::Vulkan::VulkanInitParams vulkanInitParams;
    if(window)
    {
        vulkanInitParams.requiredExtensions = window->requiredExtensions();
        // This is used to keep the Vulkan initialization uninterrupted in constructor, the surface and resolution is obtained via function pointer from window
        vulkanInitParams.surface = std::bind(&Window::Window::getSurface, window.get(), std::placeholders::_1);
        vulkanInitParams.currentResolution = std::bind(&Window::Window::resolution, window.get());
        vulkanInitParams.resolution = window->resolution();
        vulkanInitParams.shaderCodes.vertex = shader->loadFromFile("fullScreenVS");
        vulkanInitParams.shaderCodes.fragment = shader->loadFromFile("splitScreenFS");
        
        std::shared_ptr<Loader::Image> image = std::make_shared<Loader::ImageFfmpeg>("../resources/texture.png");
    }
    gpu = std::make_unique<Gpu::Vulkan>(vulkanInitParams);

    std::vector<uint32_t> uniforms(1, 0);
    float test = 0.0f;

    if(window)
    {
        bool end = false;
        while(!end)
        {
            window->run();
            end = window->key("Escape") || window->quit();
            test+= 0.01f; uniforms[0] = *reinterpret_cast<uint32_t*>(&test);
            gpu->updateUniformBuffer(uniforms);
            gpu->draw();
            if(window->resized())
                gpu->resize();
        }
    }
}

App::~App()
{
}
