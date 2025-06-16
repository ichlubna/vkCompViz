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
        // This is used to keep the Vulkan initialization uninterrupted in constructor, the surface is obtained via function pointer from window
        vulkanInitParams.surface = std::bind(&Window::Window::getSurface, window.get(), std::placeholders::_1);
        auto resolution = window->resolution();
        vulkanInitParams.resolution = {resolution.x, resolution.y};
        vulkanInitParams.shaderCodes.vertex = shader->loadFromFile("fullScreenVS");
        vulkanInitParams.shaderCodes.fragment = shader->loadFromFile("splitScreenFS");
    }
    gpu = std::make_unique<Gpu::Vulkan>(vulkanInitParams);

    if(window)
    {
        bool end = false;
        while(!end)
        {
            window->run();
            end = window->key("Escape") || window->quit();
        }
    }
}

App::~App()
{
}
