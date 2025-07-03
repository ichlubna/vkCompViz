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

[[nodiscard]] std::vector<std::string> split(std::string input, char delimiter)
{
    input += delimiter;
    std::vector<std::string> result;
    size_t position = 0;
    while(position < input.size())
    {
        position = input.find(delimiter);
        auto token = input.substr(0, position);
        result.push_back(token);
        input.erase(0, position+1);
    }
    return result;
}

float App::ParameterParser::get(std::string name, float defaultValue)
{
    if(parametersMap.find(name) == parametersMap.end())
        return defaultValue;
    return parametersMap[name];
}

void App::ParameterParser::read()
{
    std::cout << "Enter parameters as a=1.0 b=2.0..." << std::endl;
    if(!lastName.empty())
        std::cout << "Last parameter was " << lastName << " you can enter only value to update it" << std::endl;
    parametersMap.clear();
    std::string input;
    std::getline(std::cin, input);
    
    if(!lastName.empty())
    {
        bool singleValue = true;
        try
        {
            std::stof(input);
        }
        catch(...)
        {
           singleValue = false; 
        }
        if(singleValue)
        {
            parametersMap[lastName] = std::stof(input);
            return;
        }
    }

    auto parameters = split(input, ' ');
    for(auto parameter : parameters)
    {
        auto nameValue = split(parameter, '=');
        if(nameValue.size() != 2)
        {
            std::cout << "Invalid parameter format, use a=1.0 b=2.0..." << std::endl;
            parametersMap.clear();
            return;
        }
        parametersMap[nameValue[0]] = std::stof(nameValue[1]);
        if(parameters.size() == 1)
            lastName = nameValue[0];
    }
    if(parameters.size() != 1)
        lastName.clear();
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
    ParameterParser parameters;

    if(window)
    {
        bool end = false;
        while(!end)
        {
            window->run();
            end = window->key("Escape") || window->quit();
            test+= 0.001f; uniforms[0] = *reinterpret_cast<uint32_t*>(&test);
            gpu->updateUniformBuffer(uniforms);
            gpu->draw();
            if(window->resized())
                gpu->resize();
            if(window->key("space"))
            {
                parameters.read();
                test = parameters.get("a", test);
            }
        }
    }
}

App::~App()
{
}
