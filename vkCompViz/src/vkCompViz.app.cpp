module vkCompViz;
import parameterParser;
import timer;
import std;
using namespace vkCompViz;

App::App() : shader{std::make_unique<Shader::SlangFactory>()}
{
}

void App::windowInit()
{
    Window::Parameters windowParameters;
    windowParameters.title = parameters.window.title;
    windowParameters.resolution = parameters.window.resolution;
    window = std::make_unique<Window::Glfw>(windowParameters);

    vulkanInitParams.requiredExtensions = window->requiredExtensions();
    // This is used to keep the Vulkan initialization uninterrupted in constructor, the surface and resolution is obtained via function pointer from window
    vulkanInitParams.surface = std::bind(&Window::Window::getSurface, window.get(), std::placeholders::_1);
    vulkanInitParams.currentResolution = std::bind(&Window::Window::resolution, window.get());
    vulkanInitParams.resolution = window->resolution();
}

void App::initShaders()
{
    vulkanInitParams.shaders.vertex = shader->loadFromFile(parameters.shaders.vertex);
    vulkanInitParams.shaders.fragment = shader->loadFromFile(parameters.shaders.fragment);
    vulkanInitParams.shaders.vertexCount = parameters.shaders.vertexCount;
    if (parameters.shaders.compute.empty())
        throw std::runtime_error("No compute shader specified");
    if (parameters.shaders.compute.size() != parameters.shaders.workGroupCounts.size())
        throw std::runtime_error("The number of compute shaders must be equal to the number of work group counts");
    for(auto &computeShader : parameters.shaders.compute)
        vulkanInitParams.shaders.compute.push_back(shader->loadFromFile(computeShader));
}

void App::initTextures()
{
    std::uint8_t dummyData[4] = {255, 0, 0, 255};
    auto dummy = std::make_shared<Loader::ImageFfmpeg>(1, 1, 1, Loader::Image::Format::RGBA_8_INT, dummyData);

    std::vector<std::shared_ptr<Loader::Image >> inputTextures;
    if(parameters.textures.input.empty())
        vulkanInitParams.textures.input.push_back(dummy); 
    else
        for(auto &texture : parameters.textures.input)
        {
            inputTextures.push_back(std::make_shared<Loader::ImageFfmpeg>(texture));
            vulkanInitParams.textures.input.push_back(inputTextures.back());
        }

    if(parameters.textures.output.empty())
        vulkanInitParams.textures.output.push_back(dummy); 
    for(auto &texture : parameters.textures.output)
    {
        std::size_t width = texture.resolution.width;
        std::size_t height = texture.resolution.height;
        if(texture.sameResolutionAsInputID != -1)
        {
            auto input = inputTextures[texture.sameResolutionAsInputID];
            width = input->width();
            height = input->height();
        }
        Loader::Image::Format format = Loader::Image::Format::RGBA_8_INT;
        if(texture.floatFormat)
            format = Loader::Image::Format::RGBA_32_FLOAT;
        if(texture.sameFormatAsInputID != -1)
        {
            auto input = inputTextures[texture.sameFormatAsInputID];
            format = input->imageFormat();
        }
        vulkanInitParams.textures.output.push_back(std::make_shared<Loader::ImageFfmpeg>(width, height, 0, format, nullptr));
    }
}

void App::initUniforms() const
{
    for(auto &uniform : parameters.shaders.uniforms)
        gpu->updateUniform(uniform.first, uniform.second);
}

void App::mainLoop()
{
    if(parameters.window.enable)
    {
        float frameTime = (1.0f/parameters.window.fps)*1000.0f;
        ParameterParser inputParameters;
        bool end = false;
        bool screenshotTaken = false;
        while(!end)
        {
            Timer timer;
            window->run();
            end = window->key("Escape") || window->quit();
            gpu->computeSettings(parameters.shaders.workGroupCounts, parameters.benchmark.enable);
            gpu->run();
            if(window->resized())
                gpu->resize();
            if(window->key("space"))
            {
                inputParameters.read();
                auto params = inputParameters.get();
                for(auto &p : params)
                    gpu->updateUniform(p.first, p.second);
            }
            if(window->key("F1") && !screenshotTaken)
            {
                screenshotTaken = true;
                const auto now = std::chrono::system_clock::now();
                auto name = std::format("{:%d-%m-%Y %H:%M:%OS}", now); 
                auto path = (std::filesystem::path(parameters.screenshot.path) / std::filesystem::path(name)).string() + parameters.screenshot.extension;
                saveResultImage(path); 
                std::cout << "Screenshot saved to " << path << std::endl; 
            }
            else if(!window->key("F1"))
                screenshotTaken = false;

            for(auto const& binding : parameters.keyBindings)
            {
                if(window->key(binding.keyIncrease))
                    gpu->addToUniform(binding.uniform, binding.step);
                if(window->key(binding.keyDecrease))
                    gpu->addToUniform(binding.uniform, -binding.step);
            }
            timer.waitUntilElapsed(frameTime);
        }
    }
    else
    {
        for(std::size_t i = 0; i < parameters.shaders.iterations; i++)
        {
            gpu->computeSettings(parameters.shaders.workGroupCounts, parameters.benchmark.enable);
            gpu->run();
        }
    }
}

void App::initShaderStorageBuffer()
{
    vulkanInitParams.shaderStorageBuffer.size = parameters.shaders.storageBuffer.size;
    vulkanInitParams.shaderStorageBuffer.initialData = parameters.shaders.storageBuffer.initialData;
}

void App::run(App::Parameters const &inputParameters)
{
    parameters = inputParameters;
    vulkanInitParams.window = parameters.window.enable;
    if(parameters.window.enable)
        windowInit();
    vulkanInitParams.deviceUUID = parameters.priorityUUID;
    initShaders();
    initTextures();
    initShaderStorageBuffer();
    gpu = std::make_unique<Gpu::Vulkan>(vulkanInitParams);
    initUniforms();
    mainLoop();
}

const Resolution App::getImageResolution(std::string path) const
{
    Loader::ImageFfmpeg image(path);
    return {static_cast<std::uint32_t>(image.width()), static_cast<std::uint32_t>(image.height())};
}

const Shader::Shader::Info::WorkGroupSize App::getShaderWorkGroupSize(std::string path) const
{
    auto compiledShader = shader->loadFromFile(path);
    return {compiledShader.workGroupSize};
}

const Gpu::Gpu::WorkGroupCount App::calculateWorkGroupCount(Shader::Shader::Info::WorkGroupSize workGroupSize, Shader::Shader::Info::ThreadCount threadCount) const
{
    return {static_cast<std::size_t>(std::ceil(static_cast<float>(threadCount.x) / workGroupSize.x)),
            static_cast<std::size_t>(std::ceil(static_cast<float>(threadCount.y) / workGroupSize.y)),
            static_cast<std::size_t>(std::ceil(static_cast<float>(threadCount.z) / workGroupSize.z))};
}

void App::saveResultImage(std::string path) const
{
    gpu->resultTexture()->save(path);
}

App::~App()
{
}
