module;
#include <vulkan/vulkan.hpp>
#define VMA_VULKAN_VERSION 1004000
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
module gpu;
import vulkan_hpp;
using namespace Gpu;

Vulkan::Vulkan(VulkanInitParams params) :
    createInfo{*this, params},
    context{},
    instance{context, createInfo.instance()},
    physicalDevice{createInfo.bestPhysicalDevice()},
    device{physicalDevice, createInfo.device()},
    memory{instance, physicalDevice, device, *this},
    queues{ .graphics{device.getQueue(createInfo.graphicsQueueID(), 0)},
            .compute{device.getQueue(createInfo.computeQueueID(), 0)},
            .present{device.getQueue(createInfo.presentQueueID(), 0)}},
    commandPools{   .graphics{device, createInfo.commandPool(createInfo.graphicsQueueID())},
                    .compute{device, createInfo.commandPool(createInfo.computeQueueID())}},
    sampler{device, createInfo.sampler()},
    descriptorSetLayout{device, createInfo.descriptorSetLayout(params.textures.input.size(), params.textures.output.size())},
    swapChain{device, createInfo.swapChain(), createInfo.windowEnabled()},
    currentUniformBufferData{static_cast<std::uint32_t>(params.shaders.uniformBufferUint32Count()), 0},
    uniformNames{params.shaders.uniformNames()},
    shaders{.vertex{device, createInfo.shaderModule(params.shaders.vertex.code)},
            .fragment{device, createInfo.shaderModule(params.shaders.fragment.code)},
            .compute{createComputeShaders(params.shaders.compute)}},
    pipelines{  .graphics{device, createInfo},
                .compute{shaders.compute, device, createInfo}}
{
    init();
}

VkSurfaceKHR Vulkan::CreateInfo::surface()
{
    if (windowEnabled()) 
        return reinterpret_cast<VkSurfaceKHR>(params.surface(reinterpret_cast<std::uintptr_t>(static_cast<VkInstance>(*vulkan.instance))));
    else
        throw std::runtime_error("Window not enabled but requesting surface");
}

template<typename T>
bool areVectorsSame(const std::vector<T> &a, const std::vector<T> &b)
{
    if(a.size() != b.size())
        return false;
    for(size_t i = 0; i < a.size(); i++)
        if(a[i] != b[i])
            return false;
    return true;
}

std::vector<std::string> Vulkan::VulkanInitParams::Shaders::uniformNames() const
{
    if(compute.size() == 0)
        return {};

    for(size_t i = 1; i < compute.size(); i++)
        if(!areVectorsSame(compute[0].uniformNames, compute[i].uniformNames))
            throw std::runtime_error("The uniform buffer does not contain the same member variables in all compute shaders");
    return compute[0].uniformNames;
}

size_t Vulkan::VulkanInitParams::Shaders::uniformBufferSize() const
{
    size_t size = 0;
    for(auto& computeShader : compute)
        size = std::max(computeShader.uniformBufferSize, size);
    return size;
}

vk::ApplicationInfo &Vulkan::CreateInfo::application()
{
    applicationInfo
    .setPApplicationName("My Vulkan App")
    .setPEngineName("My Engine")
    .setApiVersion(VK_API_VERSION_1_4)
    .setPNext(nullptr);
    return applicationInfo;
}

std::vector<const char *> &Vulkan::CreateInfo::allInstanceExtensions()
{
    allExtensions.clear();
    allExtensions.insert(allExtensions.end(), params.requiredExtensions.begin(), params.requiredExtensions.end());
    allExtensions.insert(allExtensions.end(), instanceExtensions.begin(), instanceExtensions.end());
    return allExtensions;
}

vk::InstanceCreateInfo &Vulkan::CreateInfo::instance()
{
    instanceCreateInfo
    .setPApplicationInfo(&application())
    .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
    .setPEnabledExtensionNames(allInstanceExtensions());
    if(DEBUG)
    {
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        instanceCreateInfo.enabledLayerCount = validationLayers.size();
    }
    return instanceCreateInfo;
}

class DeviceRating
{
    public:
        DeviceRating(const vk::raii::PhysicalDevice *testedDevice, const vk::SurfaceKHR *testedSurface, const std::vector<const char*> &requiredExtensions, bool windowEnabled, std::string priorityUUID);
        DeviceRating() = default;
        void printInfo();
        const vk::raii::PhysicalDevice *device;
        std::string name;
        std::string id;
        std::set<std::string> uniqueCapabilities;
        bool priority{false};
        class QueueIndices
        {
            public:
                std::size_t graphics{0};
                std::size_t compute{0};
                std::size_t present{0};
        } queueIndex;
        size_t score{0};
        static bool extensionPresent(const std::vector<const char*> &requiredExtensions, const std::vector<vk::ExtensionProperties> &extensionProperties);
        bool swapChainSupport(const vk::SurfaceKHR &testedSurface);
};
inline bool operator>(const DeviceRating& a, const DeviceRating& b)
{
    return a.score > b.score;
}

void DeviceRating::printInfo()
{
    std::cout << "Name: " << name << std::endl;
    std::cout << "- Capabilities: ";
    for(const auto& capability : uniqueCapabilities)
        std::cout << capability << " ";
    std::cout << std::endl;
    std::cout << "- ID: " << id << std::endl;
    if(priority)
        std::cout << "- Prioritized by the user" << std::endl;
}

bool DeviceRating::extensionPresent(const std::vector<const char*> &requiredExtensions, const std::vector<vk::ExtensionProperties> &extensionProperties)
{
    bool allExtensionsSupported = true;
    for(const auto& requiredExtension : requiredExtensions)
    {
        bool isAvailable = false;
        for(const auto& extensionProperty : extensionProperties)
            if(std::string(extensionProperty.extensionName) == requiredExtension)
            {
                isAvailable = true;
                break;
            }
        if(!isAvailable)
        {
            allExtensionsSupported = false;
            break;
        }
    }
    return allExtensionsSupported;
}

bool DeviceRating::swapChainSupport(const vk::SurfaceKHR &testedSurface)
{
    auto surfaceFormats = device->getSurfaceFormatsKHR(testedSurface);
    auto surfacePresentModes = device->getSurfacePresentModesKHR(testedSurface);
    return !surfaceFormats.empty() && !surfacePresentModes.empty();
}

DeviceRating::DeviceRating(const vk::raii::PhysicalDevice *testedDevice, const vk::SurfaceKHR *testedSurface, const std::vector<const char*> &requiredExtensions, bool windowEnabled, std::string priorityID) : device{testedDevice}
{
    const auto properties = device->getProperties();
    name = std::string(properties.deviceName);
    if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
    {
        score++;
        uniqueCapabilities.insert("Discrete");
    }

    const auto extensionProperties = device->enumerateDeviceExtensionProperties();
    if(extensionPresent(requiredExtensions, extensionProperties))
    {
        score++;
        uniqueCapabilities.insert("Required-extensions");
        if(windowEnabled)
            if(swapChainSupport(*testedSurface))
            {
                score++;
                uniqueCapabilities.insert("Swapchain");
            }
    }

    const auto queueFamilyProperties = device->getQueueFamilyProperties();
    for(auto [queueFamilyID, queueFamily] : queueFamilyProperties | std::ranges::views::enumerate)
    {
        if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            score++;
            uniqueCapabilities.insert("Graphics");
            queueIndex.graphics = queueFamilyID;
        }
        if(queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
        {
            score++;
            uniqueCapabilities.insert("Compute");
            queueIndex.compute = queueFamilyID;
        }
        if(windowEnabled)
            if(device->getSurfaceSupportKHR(queueFamilyID, *testedSurface))
            {
                score++;
                uniqueCapabilities.insert("Present");
                queueIndex.present = queueFamilyID;
            }
    }

    for(size_t i = 0; i < VK_UUID_SIZE; i++)
        id += std::to_string(properties.pipelineCacheUUID[i]);
    if(id == priorityID)
    {
        score += 1000;
        priority = true;
    }
}

vk::raii::PhysicalDevice Vulkan::CreateInfo::bestPhysicalDevice()
{
    if(windowEnabled())
    {
        vulkan.surface.emplace(vulkan.instance, surface());
        deviceExtensions.push_back("VK_KHR_swapchain");
    }
    vk::raii::PhysicalDevices physicalDevices(vulkan.instance);
    if(physicalDevices.empty())
        throw std::runtime_error("No physical devices found");

    if(!params.deviceUUID.empty())
    {
        std::cout << "Setting priority to device with UUID: " << params.deviceUUID << " if present" << std::endl;
    }

    vk::PhysicalDevice bestPhysicalDevice;
    DeviceRating bestDeviceRating;
    std::cout << "The following devices are available:" << std::endl;
    std::set<std::string> uniqueIDs;
    for(const auto& currentDevice : physicalDevices)
    {
        const vk::SurfaceKHR *testedSurface = (windowEnabled()) ? &(*vulkan.surface.value()) : nullptr;
        DeviceRating deviceRating{&currentDevice, testedSurface, deviceExtensions, windowEnabled(), params.deviceUUID};
        auto [iterator, inserted] = uniqueIDs.insert(deviceRating.id);
        if(inserted)
        {
            deviceRating.printInfo();
            if(deviceRating > bestDeviceRating)
                bestDeviceRating = deviceRating;
        }
    }
    std::cout << std::endl;
    if(bestDeviceRating.score > 0)
    {
        queueIndex.compute = bestDeviceRating.queueIndex.compute;
        queueIndex.graphics = bestDeviceRating.queueIndex.graphics;
        queueIndex.present = bestDeviceRating.queueIndex.present;
        if(uniqueIDs.size() > 1)
        {
            std::cout << "Selected as the most suitable: " << std::endl;
            bestDeviceRating.printInfo();
            std::cout << std::endl;
        }
        return *bestDeviceRating.device;
    }
    throw std::runtime_error("No suitable GPU found");
}

std::vector<vk::DeviceQueueCreateInfo> &Vulkan::CreateInfo::queues()
{
    std::set<std::size_t> uniqueIDs = {queueIndex.graphics, queueIndex.compute, queueIndex.present};
    for(auto& id : uniqueIDs)
        queueCreateInfos.emplace_back()
                        .setQueueCount(1)
                        .setPQueuePriorities(&queuePriorities)
                        .setQueueFamilyIndex(id);
    return queueCreateInfos;
}

vk::DeviceCreateInfo &Vulkan::CreateInfo::device()
{
    physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
    physicalDeviceVulkan12Features.runtimeDescriptorArray = VK_TRUE;
    physicalDeviceComputeShaderDerivativesFeatures.computeDerivativeGroupQuads = VK_TRUE;
    physicalDeviceVulkan12Features.setPNext(&physicalDeviceComputeShaderDerivativesFeatures);
    deviceCreateInfo
    .setQueueCreateInfos(queues())
    .setPEnabledExtensionNames(deviceExtensions)
    .setPEnabledFeatures(&physicalDeviceFeatures)
    .setPNext(&physicalDeviceVulkan12Features);
    return deviceCreateInfo;
}

Vulkan::Pipelines::Graphics::Graphics(vk::raii::Device &device, Vulkan::CreateInfo &createInfo)
{
    if(createInfo.windowEnabled())
    {
        layout.emplace(device, createInfo.pipelineLayout());
        renderPass.emplace(device, createInfo.renderPass());
        pipeline.emplace(device, nullptr, createInfo.graphicsPipeline());
    }
};

vk::SurfaceFormatKHR swapChainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
{
    for(const auto& availableFormat : availableFormats)
        if(availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return availableFormat;
    return availableFormats.front();
}

vk::PresentModeKHR swapChainPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
{
    for(const auto& availablePresentMode : availablePresentModes)
        if(availablePresentMode == vk::PresentModeKHR::eMailbox)
            return availablePresentMode;
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D swapChainExtent(const vk::SurfaceCapabilitiesKHR &capabilities, Resolution resolution)
{
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    vk::Extent2D extent = {resolution.width, resolution.height};
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

vk::SwapchainCreateInfoKHR &Vulkan::CreateInfo::swapChain()
{
    if(!windowEnabled())
        return swapChainCreateInfo;
    auto surfaceCapabilities = vulkan.physicalDevice.getSurfaceCapabilitiesKHR(vulkan.surface.value());
    auto surfaceFormat = swapChainSurfaceFormat(vulkan.physicalDevice.getSurfaceFormatsKHR(vulkan.surface.value()));
    auto presentMode = swapChainPresentMode(vulkan.physicalDevice.getSurfacePresentModesKHR(vulkan.surface.value()));
    auto extent = swapChainExtent(surfaceCapabilities, params.resolution);
    size_t imageCount = surfaceCapabilities.minImageCount + 1;
    if(surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        imageCount = surfaceCapabilities.maxImageCount;

    swapChainCreateInfo
    .setSurface(vulkan.surface.value())
    .setMinImageCount(imageCount)
    .setImageFormat(surfaceFormat.format)
    .setImageColorSpace(surfaceFormat.colorSpace)
    .setImageExtent(extent)
    .setImageArrayLayers(1)
    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
    .setImageSharingMode(vk::SharingMode::eExclusive)
    .setPreTransform(surfaceCapabilities.currentTransform)
    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
    .setPresentMode(presentMode)
    .setClipped(true)
    .setOldSwapchain(nullptr);
    return swapChainCreateInfo;
}

Vulkan::SwapChain::SwapChain(vk::raii::Device &device, const vk::SwapchainCreateInfoKHR &swapChainCreateInfo, bool windowEnabled)
{
    if(windowEnabled)
        swapChain.emplace(device, swapChainCreateInfo);
    extent = swapChainCreateInfo.imageExtent;
    imageFormat = swapChainCreateInfo.imageFormat;
}

vk::ShaderModuleCreateInfo &Vulkan::CreateInfo::shaderModule(std::vector<uint32_t> &code)
{
    shaderModuleCreateInfos.emplace_back()
                           .setCodeSize(code.size() * sizeof(uint32_t))
                           .setPCode(code.data());
    return shaderModuleCreateInfos.back();
}

vk::PipelineShaderStageCreateInfo &Vulkan::CreateInfo::pipelineShaderStage(vk::raii::ShaderModule &shaderModule, vk::ShaderStageFlagBits stage)
{
    pipelineShaderStageCreateInfos.emplace_back()
                                  .setStage(vk::ShaderStageFlagBits(stage))
                                  .setModule(shaderModule)
                                  .setPName("main");
    return pipelineShaderStageCreateInfos.back();
}

vk::PipelineDynamicStateCreateInfo &Vulkan::CreateInfo::pipelineDynamic()
{
    dynamicStateCreateInfo.setDynamicStates(dynamicStates);
    return dynamicStateCreateInfo;
}

vk::PipelineVertexInputStateCreateInfo &Vulkan::CreateInfo::vertexInput()
{
    vertexInputCreateInfo
    .setVertexBindingDescriptions({})
    .setVertexAttributeDescriptions({});
    return vertexInputCreateInfo;
}

vk::PipelineInputAssemblyStateCreateInfo &Vulkan::CreateInfo::inputAssembly()
{
    inputAssemblyCreateInfo
    .setTopology(vk::PrimitiveTopology::eTriangleList)
    .setPrimitiveRestartEnable(false);
    return inputAssemblyCreateInfo;
}

vk::PipelineViewportStateCreateInfo &Vulkan::CreateInfo::viewport()
{
    pipelineViewport
    .setX(0.0f)
    .setY(0.0f)
    .setWidth(vulkan.swapChain.extent.width)
    .setHeight(vulkan.swapChain.extent.height)
    .setMinDepth(0.0f)
    .setMaxDepth(1.0f);

    pipelineScissor
    .setOffset(vk::Offset2D{0, 0})
    .setExtent(vulkan.swapChain.extent);

    viewportCreateInfo
    .setViewports(pipelineViewport)
    .setScissors(pipelineScissor);
    return viewportCreateInfo;
}

vk::PipelineRasterizationStateCreateInfo &Vulkan::CreateInfo::rasterization()
{
    rasterizationCreateInfo
    .setDepthClampEnable(false)
    .setRasterizerDiscardEnable(false)
    .setPolygonMode(vk::PolygonMode::eFill)
    .setLineWidth(1.0f)
    .setCullMode(vk::CullModeFlagBits::eBack)
    .setFrontFace(vk::FrontFace::eClockwise)
    .setDepthBiasEnable(false)
    .setDepthBiasConstantFactor(0.0f)
    .setDepthBiasClamp(0.0f)
    .setDepthBiasSlopeFactor(0.0f);
    return rasterizationCreateInfo;
}

vk::PipelineMultisampleStateCreateInfo &Vulkan::CreateInfo::multisample()
{
    multisampleCreateInfo
    .setSampleShadingEnable(false)
    .setRasterizationSamples(vk::SampleCountFlagBits::e1)
    .setMinSampleShading(1.0f)
    .setPSampleMask(nullptr)
    .setAlphaToCoverageEnable(false)
    .setAlphaToOneEnable(false);
    return multisampleCreateInfo;
}

vk::PipelineColorBlendAttachmentState &Vulkan::CreateInfo::colorBlendAttachment()
{
    colorBlendAttachmentState
    .setBlendEnable(false)
    .setSrcColorBlendFactor(vk::BlendFactor::eOne)
    .setDstColorBlendFactor(vk::BlendFactor::eZero)
    .setColorBlendOp(vk::BlendOp::eAdd)
    .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
    .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
    .setAlphaBlendOp(vk::BlendOp::eAdd)
    .setColorWriteMask(vk::ColorComponentFlagBits::eR
                       | vk::ColorComponentFlagBits::eG
                       | vk::ColorComponentFlagBits::eB
                       | vk::ColorComponentFlagBits::eA);
    return colorBlendAttachmentState;
}

vk::PipelineColorBlendStateCreateInfo &Vulkan::CreateInfo::colorBlend()
{
    colorBlendCreateInfo
    .setLogicOpEnable(false)
    .setLogicOp(vk::LogicOp::eCopy)
    .setAttachmentCount(1)
    .setPAttachments(&colorBlendAttachment())
    .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
    return colorBlendCreateInfo;
}

vk::PipelineLayoutCreateInfo &Vulkan::CreateInfo::pipelineLayout()
{
    pipelineLayoutCreateInfo
    .setSetLayoutCount(1)
    .setPSetLayouts(&*vulkan.descriptorSetLayout)
    .setPushConstantRangeCount(0)
    .setPPushConstantRanges(nullptr);
    return pipelineLayoutCreateInfo;
}

vk::RenderPassCreateInfo &Vulkan::CreateInfo::renderPass()
{
    colorAttachment
    .setFormat(vulkan.swapChain.imageFormat)
    .setSamples(vk::SampleCountFlagBits::e1)
    .setLoadOp(vk::AttachmentLoadOp::eClear)
    .setStoreOp(vk::AttachmentStoreOp::eStore)
    .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
    .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
    .setInitialLayout(vk::ImageLayout::eUndefined)
    .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    colorAttachmentReference
    .setAttachment(0)
    .setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    subpass
    .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
    .setColorAttachmentCount(1)
    .setPColorAttachments(&colorAttachmentReference);

    subpassDependency
    .setSrcSubpass(VK_SUBPASS_EXTERNAL)
    .setDstSubpass(0)
    .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    .setSrcAccessMask(vk::AccessFlagBits::eNone)
    .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

    renderPassCreateInfo
    .setAttachmentCount(1)
    .setPAttachments(&colorAttachment)
    .setSubpassCount(1)
    .setPSubpasses(&subpass)
    .setDependencyCount(1)
    .setPDependencies(&subpassDependency);
    return renderPassCreateInfo;
}

vk::GraphicsPipelineCreateInfo &Vulkan::CreateInfo::graphicsPipeline()
{
    shaderStages.clear();
    shaderStages.emplace_back(pipelineShaderStage(vulkan.shaders.vertex, vk::ShaderStageFlagBits::eVertex));
    shaderStages.emplace_back(pipelineShaderStage(vulkan.shaders.fragment, vk::ShaderStageFlagBits::eFragment));

    graphicsPipelineCreateInfo
    .setStages(shaderStages)
    .setPVertexInputState(&vertexInput())
    .setPInputAssemblyState(&inputAssembly())
    .setPViewportState(&viewport())
    .setPRasterizationState(&rasterization())
    .setPMultisampleState(&multisample())
    .setPColorBlendState(&colorBlend())
    .setPDepthStencilState(nullptr)
    .setPDynamicState(&pipelineDynamic())
    .setLayout(vulkan.pipelines.graphics.layout.value())
    .setRenderPass(vulkan.pipelines.graphics.renderPass.value())
    .setSubpass(0)
    .setBasePipelineHandle(VK_NULL_HANDLE)
    .setBasePipelineIndex(-1);
    return graphicsPipelineCreateInfo;
}

vk::ComputePipelineCreateInfo &Vulkan::CreateInfo::computePipeline(vk::raii::ShaderModule &shaderModule)
{ 
    shaderStage
    .setModule(shaderModule)
    .setPName("main")
    .setStage(vk::ShaderStageFlagBits::eCompute); 

    computePipelineCreateInfo
    .setStage(shaderStage)
    .setLayout(vulkan.pipelines.compute.layout);
    return computePipelineCreateInfo;
}

vk::FramebufferCreateInfo &Vulkan::CreateInfo::frameBuffer(vk::raii::ImageView &attachment)
{
    frameBufferAttachments = {attachment};
    frameBufferCreateInfo
    .setRenderPass(vulkan.pipelines.graphics.renderPass.value())
    .setAttachments(frameBufferAttachments)
    .setWidth(vulkan.swapChain.extent.width)
    .setHeight(vulkan.swapChain.extent.height)
    .setLayers(1);
    return frameBufferCreateInfo;
}

vk::CommandPoolCreateInfo &Vulkan::CreateInfo::commandPool(size_t queueFamilyID)
{
    commandPoolCreateInfo
    .setQueueFamilyIndex(queueFamilyID)
    .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    return commandPoolCreateInfo;
}

vk::CommandBufferAllocateInfo &Vulkan::CreateInfo::commandBuffer(vk::raii::CommandPool &commandPool, uint32_t count)
{
    commandBufferAllocateInfo
    .setCommandPool(commandPool)
    .setLevel(vk::CommandBufferLevel::ePrimary)
    .setCommandBufferCount(count);
    return commandBufferAllocateInfo;
}

vk::RenderPassBeginInfo &Vulkan::CreateInfo::renderPassBegin(vk::raii::Framebuffer &frameBuffer)
{
    renderPassBeginInfo
    .setRenderPass(vulkan.pipelines.graphics.renderPass.value())
    .setFramebuffer(frameBuffer)
    .setRenderArea({{0, 0}, vulkan.swapChain.extent})
    .setClearValueCount(1)
    .setPClearValues(&clearColor);
    return renderPassBeginInfo;
}

vk::SemaphoreCreateInfo &Vulkan::CreateInfo::semaphore()
{
    return semaphoreCreateInfo;
}

vk::FenceCreateInfo &Vulkan::CreateInfo::fence()
{
    fenceCreateInfo
    .setFlags(vk::FenceCreateFlagBits::eSignaled);
    return fenceCreateInfo;
}

vk::DescriptorSetLayoutCreateInfo &Vulkan::CreateInfo::descriptorSetLayout(size_t inputTextureCount, size_t outputTextureCount)
{
    auto allStages = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment | vk::ShaderStageFlagBits::eCompute;    

    bindings.clear();
    bindings.emplace_back()
    .setBinding(Bindings::UNIFORM)
    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
    .setDescriptorCount(1)
    .setStageFlags(allStages);
  
    bindings.emplace_back()
    .setBinding(Bindings::OUTPUT_TEXTURE_SAMPLER)
    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
    .setDescriptorCount(outputTextureCount) 
    .setStageFlags(allStages); 
    
    bindings.emplace_back()
    .setBinding(Bindings::OUTPUT_TEXTURE_STORAGE)
    .setDescriptorType(vk::DescriptorType::eStorageImage)
    .setDescriptorCount(outputTextureCount) 
    .setStageFlags(allStages); 
    
    bindings.emplace_back()
    .setBinding(Bindings::INPUT_TEXTURE_SAMPLER)
    .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
    .setDescriptorCount(inputTextureCount) 
    .setStageFlags(allStages); 
    
    bindings.emplace_back()
    .setBinding(Bindings::INPUT_TEXTURE)
    .setDescriptorType(vk::DescriptorType::eSampledImage)
    .setDescriptorCount(inputTextureCount) 
    .setStageFlags(allStages); 
    
    bindings.emplace_back()
    .setBinding(Bindings::SHADER_STORAGE)
    .setDescriptorType(vk::DescriptorType::eStorageBuffer)
    .setDescriptorCount(1)
    .setStageFlags(allStages); 

    descriptorSetLayoutCreateInfo
    .setBindings(bindings);
    return descriptorSetLayoutCreateInfo;
}

Vulkan::Memory::Memory(vk::raii::Instance &instance, vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device, Vulkan &vulkan) : vulkan{vulkan}
{
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.instance = *instance;
    allocatorInfo.physicalDevice = *physicalDevice;
    allocatorInfo.device = *device;
    if(vmaCreateAllocator(&allocatorInfo, &allocator))
        throw std::runtime_error("Failed to create allocator");
}

std::vector<vk::raii::ShaderModule> Vulkan::createComputeShaders(std::vector<Shader::Shader::Info> &computeShaders)
{
    std::vector<vk::raii::ShaderModule> shaders;
    shaders.reserve(computeShaders.size());
    for(auto &shader : computeShaders)
        shaders.push_back({device, createInfo.shaderModule(shader.code)});
    return shaders;
}

Vulkan::Pipelines::Compute::Compute(std::vector<vk::raii::ShaderModule> &shaders, vk::raii::Device &device, CreateInfo &createInfo) : layout{device, createInfo.pipelineLayout()}
{ 
    for (auto &shader : shaders)
        pipelines.push_back({device, nullptr, createInfo.computePipeline(shader)});
}

void Vulkan::createInputTextures()
{
    auto inputImages = createInfo.inputImages();
    for(auto const &inputImage : inputImages)
       inputTextures.emplace_back(memory.texture(inputImage)); 
}

void Vulkan::init()
{
    createSwapChainFrames();
}

void Vulkan::recordGraphicsCommandBuffer(SwapChain::Frame &frame, SwapChain::InFlight &inFlight)
{
    auto &buffer = *inFlight.commandBuffers.graphics;
    buffer.begin({});
    buffer.beginRenderPass(createInfo.renderPassBegin(*frame.frameBuffer), vk::SubpassContents::eInline);
    buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.graphics.pipeline.value());
    buffer.setViewport(0, {vk::Viewport(0, 0, swapChain.extent.width, swapChain.extent.height, 0, 1)});
    buffer.setScissor(0, {vk::Rect2D({0, 0}, swapChain.extent)});
    buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelines.graphics.layout.value(), 0, {inFlight.descriptorSet.value()}, {});
    buffer.draw(createInfo.vertexCount(), 1, 0, 0);
    buffer.endRenderPass();
    buffer.end();
}

void Vulkan::graphicsSubmit(size_t swapChainFrameID)
{
    auto &inFlight = swapChain.currentInFlight();
    device.resetFences({*inFlight.fences.inFlight});
    
    recordGraphicsCommandBuffer(swapChain.frames[swapChainFrameID], inFlight);

    std::vector<vk::PipelineStageFlags> waitStage{vk::PipelineStageFlagBits::eVertexInput, vk::PipelineStageFlagBits::eColorAttachmentOutput};
    std::vector<vk::Semaphore> waitSemaphores{*inFlight.semaphores.computeFinished.value(), *inFlight.semaphores.imageAvailable.value()};
    vk::SubmitInfo submitInfo;
    submitInfo
    .setCommandBuffers({*inFlight.commandBuffers.graphics.value()})
    .setSignalSemaphores({*inFlight.semaphores.renderFinished.value()})
    .setWaitSemaphores(waitSemaphores)
    .setWaitDstStageMask(waitStage);

    queues.graphics.submit({submitInfo}, *inFlight.fences.inFlight);
}

void Vulkan::updateUniformBuffer(SwapChain::InFlight &inFlight)
{
    vmaCopyMemoryToAllocation(*inFlight.buffers.uniform->allocator, currentUniformBufferData.data(), inFlight.buffers.uniform->allocation, 0, currentUniformBufferData.size()*sizeof(uint32_t));
}

void Vulkan::updateShaderStorageBuffer(SwapChain::InFlight &inFlight)
{
    if(createInfo.shaderStorageBufferData().empty())
        return;
    vmaCopyMemoryToAllocation(*inFlight.buffers.shaderStorage->allocator, createInfo.shaderStorageBufferData().data(), inFlight.buffers.shaderStorage->allocation, 0, createInfo.shaderStorageBufferData().size()*sizeof(float));
    device.waitIdle();
}

void Vulkan::draw(SwapChain::InFlight &inFlight) 
{
    if(createInfo.windowEnabled())
    {
        while(device.waitForFences({inFlight.fences.inFlight.value()}, VK_TRUE, timeout) == vk::Result::eTimeout);

        uint32_t swapChainFrameID;
        vk::Result result;
        try
        {
            std::tie(result, swapChainFrameID) = swapChain.swapChain.value().acquireNextImage(timeout, *inFlight.semaphores.imageAvailable);
        }
        catch(const vk::SystemError& err)
        {
            if(err.code() == vk::Result::eErrorOutOfDateKHR)
            {
                recreateSwapChain();
                return;
            }
            else
                throw;
        }
        graphicsSubmit(swapChainFrameID);

        vk::PresentInfoKHR presentInfo;
        presentInfo
        .setWaitSemaphores({*inFlight.semaphores.renderFinished.value()})
        .setSwapchains({*swapChain.swapChain.value()})
        .setPImageIndices(&swapChainFrameID);
        try
        {
            result = queues.present.presentKHR(presentInfo);
        }
        catch(const vk::SystemError& err)
        {
            if(err.code() == vk::Result::eErrorOutOfDateKHR || err.code() == vk::Result::eSuboptimalKHR)
            {
                recreateSwapChain();
                resizeRequired = false;
            }
            else
                throw;
        }
        if(resizeRequired)
        {
            recreateSwapChain();
            resizeRequired = false;
        }
    }
}

void Vulkan::run()
{
    auto &inFlight = swapChain.currentInFlight();
    while(device.waitForFences({inFlight.fences.computeInFlight.value()}, VK_TRUE, timeout) == vk::Result::eTimeout);
    updateUniformBuffer(inFlight);
    compute(inFlight);
    draw(inFlight);
    swapChain.nextInFlight();
}

vk::ImageViewCreateInfo &Vulkan::CreateInfo::imageView(vk::Format imageFormat, vk::Image image)
{
    imageViewCreateInfo
    .setViewType(vk::ImageViewType::e2D)
    .setFormat(imageFormat)
    .setImage(image)
    .setComponents({vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity,
                    vk::ComponentSwizzle::eIdentity})
    .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    return imageViewCreateInfo;
}

void Vulkan::CreateInfo::createFrameSync(SwapChain::InFlight &frame)
{
    frame.semaphores.imageAvailable = std::move(vk::raii::Semaphore(vulkan.device, semaphore()));
    frame.semaphores.renderFinished = std::move(vk::raii::Semaphore(vulkan.device, semaphore()));
    frame.semaphores.computeFinished = std::move(vk::raii::Semaphore(vulkan.device, semaphore()));
    frame.fences.inFlight = std::move(vk::raii::Fence(vulkan.device, fence()));
    frame.fences.computeInFlight = std::move(vk::raii::Fence(vulkan.device, fence()));
}

void Vulkan::CreateInfo::createFrameBuffer(Vulkan::SwapChain::Frame &frame, vk::Image image)
{
    frame.image = image;
    auto &imageViewCreateInfo = imageView(vulkan.swapChain.imageFormat, image);
    frame.imageView.emplace(vk::raii::ImageView{vulkan.device, imageViewCreateInfo});
    frame.frameBuffer.emplace(vk::raii::Framebuffer{vulkan.device, frameBuffer(*frame.imageView)});
}

std::unique_ptr<Vulkan::Buffer> Vulkan::Memory::buffer(vk::BufferUsageFlags usage, size_t size)
{
    auto buffer = std::make_unique<Buffer>();
    buffer->allocator = &allocator;
    vk::BufferCreateInfo bufferInfo;
    bufferInfo
    .setUsage(usage | vk::BufferUsageFlagBits::eTransferDst)
    .setSize(size);
    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    auto info = static_cast<VkBufferCreateInfo>(bufferInfo);
    vmaCreateBuffer(allocator, &info, &allocInfo, &buffer->buffer, &buffer->allocation, nullptr);
    return buffer;
}

vk::ImageCreateInfo &Vulkan::CreateInfo::image(vk::Format imageFormat, Resolution resolution, bool storage=false)
{
    queueFamilyIndices.clear();
    queueFamilyIndices.push_back(graphicsQueueID());
    queueFamilyIndices.push_back(computeQueueID());
    auto usageFlags = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    if(storage)
        usageFlags |= vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc;

    imageCreateInfo
    .setFormat(imageFormat)
    .setImageType(vk::ImageType::e2D)
    .setExtent({resolution.width, resolution.height, 1})
    .setMipLevels(1)
    .setArrayLayers(1)
    .setTiling(vk::ImageTiling::eOptimal)
    .setInitialLayout(vk::ImageLayout::eUndefined)
    .setUsage(usageFlags)
    .setSharingMode(vk::SharingMode::eConcurrent)
    .setQueueFamilyIndices(queueFamilyIndices)
    .setSamples(vk::SampleCountFlagBits::e1);
    return imageCreateInfo;
}

std::unique_ptr<Vulkan::OneTimeCommand> Vulkan::oneTimeCommand()
{
    auto command = std::make_unique<OneTimeCommand>();
    vk::raii::CommandBuffers commandBuffers(device, createInfo.commandBuffer(commandPools.graphics, 1));
    command->buffer.emplace(std::move(commandBuffers[0]));
    command->queue = &queues.graphics;
    
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    command->buffer.value().begin(beginInfo);
    return command;
}

Vulkan::OneTimeCommand::~OneTimeCommand()
{
    buffer.value().end();                        
    vk::SubmitInfo submitInfo;
    submitInfo
    .setCommandBuffers({*buffer.value()});
    queue->submit({submitInfo}, nullptr);
    queue->waitIdle();
}  

void Vulkan::copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
{ 
    auto buffer = oneTimeCommand();
    vk::BufferCopy copyRegion(0, 0, size);
    buffer->command().copyBuffer(src, dst, copyRegion);
}

vk::Format formatToVk(Loader::Image::Format format)
{
    if(format == Loader::Image::Format::RGBA_8_INT)
        return vk::Format::eR8G8B8A8Unorm;
    else
        return vk::Format::eR32G32B32A32Sfloat;
}

void Vulkan::transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto buffer = oneTimeCommand();
    vk::ImageMemoryBarrier barrier;
    barrier
    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
    .setImage(image)
    .setOldLayout(oldLayout)
    .setNewLayout(newLayout)
    .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    vk::PipelineStageFlags srcStage;
    vk::PipelineStageFlags dstStage;

    auto allStages = vk::PipelineStageFlagBits::eVertexShader | vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;

    if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = allStages;
    }
    else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eGeneral)
    {
        barrier
        .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite);
        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = allStages;
    }
    else if(oldLayout == vk::ImageLayout::eGeneral && newLayout == vk::ImageLayout::eTransferSrcOptimal)
    {
        barrier
        .setSrcAccessMask(vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite)
        .setDstAccessMask(vk::AccessFlagBits::eTransferRead);
        srcStage = allStages;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if(oldLayout == vk::ImageLayout::eTransferSrcOptimal && newLayout == vk::ImageLayout::eGeneral)
    {
        barrier
        .setSrcAccessMask(vk::AccessFlagBits::eTransferRead)
        .setDstAccessMask(vk::AccessFlagBits::eShaderRead | vk::AccessFlagBits::eShaderWrite);
        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = allStages;
    }
    else
    {
        throw std::runtime_error("Unsupported layout transition");
    }
    buffer->command().pipelineBarrier(srcStage, dstStage, {}, nullptr, nullptr, barrier);
}

void Vulkan::copyBufferToImage(vk::Buffer inputBuffer, vk::Image image, size_t width, size_t height)
{
    auto buffer = oneTimeCommand();
    vk::BufferImageCopy region(0, 0, 0, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1});
    buffer->command().copyBufferToImage(inputBuffer, image, vk::ImageLayout::eTransferDstOptimal, region);
}

void Vulkan::copyImageToBuffer(vk::Image image, vk::Buffer outputBuffer, size_t width, size_t height)
{
    auto buffer = oneTimeCommand();
    vk::BufferImageCopy region(0, 0, 0, {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, {0, 0, 0}, {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1});
    buffer->command().copyImageToBuffer(image, vk::ImageLayout::eTransferSrcOptimal, outputBuffer, region);
}

std::unique_ptr<Vulkan::Texture> Vulkan::Memory::texture(std::shared_ptr<Loader::Image> image, bool storage)
{
    auto stagingBuffer = buffer(vk::BufferUsageFlagBits::eTransferSrc, image->size());
    if(image->data() == nullptr)
    {
        std::vector<uint8_t> data(image->size(), 200);
        vmaCopyMemoryToAllocation(*stagingBuffer->allocator, data.data(), stagingBuffer->allocation, 0, image->size());
    }
    else
        vmaCopyMemoryToAllocation(*stagingBuffer->allocator, image->data(), stagingBuffer->allocation, 0, image->size());
   
    auto texture = std::make_unique<Texture>();
    VmaAllocationCreateInfo imageAllocInfo = {};
    imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    auto format = formatToVk(image->imageFormat());
    auto &imageCreateInfo = vulkan.createInfo.image(format, {static_cast<uint32_t>(image->width()), static_cast<uint32_t>(image->height())}, storage);
    vmaCreateImage(allocator, imageCreateInfo, &imageAllocInfo, &texture->image, &texture->allocation, nullptr);
    vulkan.transitionImageLayout(texture->image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    vulkan.copyBufferToImage(static_cast<vk::Buffer>(stagingBuffer->buffer), texture->image, image->width(), image->height());
    if (storage)
        vulkan.transitionImageLayout(texture->image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral);
    else
        vulkan.transitionImageLayout(texture->image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    texture->view.emplace(vk::raii::ImageView{vulkan.device, vulkan.createInfo.imageView(format, texture->image)}); 
    texture->allocator = &allocator;
    return texture;
}

vk::SamplerCreateInfo &Vulkan::CreateInfo::sampler()
{  
    const auto properties = vulkan.physicalDevice.getProperties();
    samplerCreateInfo
    .setMagFilter(vk::Filter::eLinear)
    .setMinFilter(vk::Filter::eLinear)
    .setAddressModeU(vk::SamplerAddressMode::eRepeat)
    .setAddressModeV(vk::SamplerAddressMode::eRepeat)
    .setAddressModeW(vk::SamplerAddressMode::eRepeat)
    .setAnisotropyEnable(false)
    .setMaxAnisotropy(properties.limits.maxSamplerAnisotropy)
    .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
    .setUnnormalizedCoordinates(false)
    .setCompareEnable(false)
    .setCompareOp(vk::CompareOp::eAlways)
    .setMipmapMode(vk::SamplerMipmapMode::eLinear)
    .setMipLodBias(0.0f)
    .setMinLod(0.0f)
    .setMaxLod(0.0f);
    return samplerCreateInfo;
}

vk::DescriptorPoolCreateInfo &Vulkan::CreateInfo::descriptorPool(size_t inputTextureCount, size_t outputTextureCount, size_t inFlightFramesCount)
{
    descriptorPoolSizes.clear();
    descriptorPoolSizes.push_back({vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(inFlightFramesCount)});
    descriptorPoolSizes.push_back({vk::DescriptorType::eStorageBuffer, static_cast<uint32_t>(inFlightFramesCount)});
    descriptorPoolSizes.push_back({vk::DescriptorType::eCombinedImageSampler, static_cast<uint32_t>(inFlightFramesCount * (inputTextureCount + outputTextureCount))});
    descriptorPoolSizes.push_back({vk::DescriptorType::eStorageImage, static_cast<uint32_t>(inFlightFramesCount * outputTextureCount)});
    descriptorPoolSizes.push_back({vk::DescriptorType::eSampledImage, static_cast<uint32_t>(inFlightFramesCount * inputTextureCount)});
    descriptorPoolCreateInfo
    .setMaxSets(inFlightFramesCount)
    .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
    .setPoolSizes(descriptorPoolSizes);
    return descriptorPoolCreateInfo;
}

vk::DescriptorSetAllocateInfo &Vulkan::CreateInfo::descriptorSet(vk::raii::DescriptorPool &descriptorPool, size_t count)
{
    descriptorSetLayouts.clear();
    for(size_t i = 0; i < count; i++)
        descriptorSetLayouts.push_back(vulkan.descriptorSetLayout);
    descriptorSetAllocateInfo
    .setDescriptorPool(descriptorPool)
    .setSetLayouts(descriptorSetLayouts);
    return descriptorSetAllocateInfo;
}

void Vulkan::updateDescriptorSets(SwapChain::InFlight &inFlight)
{
    vk::DescriptorBufferInfo bufferInfo;
    bufferInfo
    .setBuffer(inFlight.buffers.uniform->buffer)
    .setOffset(0)
    .setRange(VK_WHOLE_SIZE);
    
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    writeDescriptorSets.emplace_back()
        .setDstSet(*inFlight.descriptorSet.value())
        .setDstBinding(Bindings::UNIFORM)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1)
        .setBufferInfo(bufferInfo);

    std::vector<vk::DescriptorImageInfo> outImageInfos;
    std::vector<vk::DescriptorImageInfo> outImageInfosStorage;
    for(auto &texture : inFlight.outputTextures)
    {
        outImageInfos.emplace_back()
            .setSampler(sampler)
            .setImageView(texture->view.value())
            .setImageLayout(vk::ImageLayout::eGeneral);
        
        outImageInfosStorage.emplace_back()
            .setImageView(texture->view.value())
            .setImageLayout(vk::ImageLayout::eGeneral);
    }

    writeDescriptorSets.emplace_back()
        .setDstSet(*inFlight.descriptorSet.value())
        .setDstBinding(Bindings::OUTPUT_TEXTURE_SAMPLER)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(outImageInfos.size())
        .setPImageInfo(outImageInfos.data());
    
    writeDescriptorSets.emplace_back()
        .setDstSet(*inFlight.descriptorSet.value())
        .setDstBinding(Bindings::OUTPUT_TEXTURE_STORAGE)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eStorageImage)
        .setDescriptorCount(outImageInfosStorage.size())
        .setPImageInfo(outImageInfosStorage.data());

    std::vector<vk::DescriptorImageInfo> inImageInfos;
    std::vector<vk::DescriptorImageInfo> inImageInfosStorage;
    for(auto &texture : inputTextures)
    {
        inImageInfos.emplace_back()
            .setSampler(sampler)
            .setImageView(texture->view.value())
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
        
        inImageInfosStorage.emplace_back()
            .setImageView(texture->view.value())
            .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    writeDescriptorSets.emplace_back()
        .setDstSet(*inFlight.descriptorSet.value())
        .setDstBinding(Bindings::INPUT_TEXTURE_SAMPLER)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(inImageInfos.size())
        .setPImageInfo(inImageInfos.data());
    
    writeDescriptorSets.emplace_back()
        .setDstSet(*inFlight.descriptorSet.value())
        .setDstBinding(Bindings::INPUT_TEXTURE)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eSampledImage)
        .setDescriptorCount(inImageInfosStorage.size())
        .setPImageInfo(inImageInfosStorage.data());
    
    vk::DescriptorBufferInfo storageBufferInfo;
    storageBufferInfo
    .setBuffer(inFlight.buffers.shaderStorage->buffer)
    .setOffset(0)
    .setRange(VK_WHOLE_SIZE);

    writeDescriptorSets.emplace_back()
        .setDstSet(*inFlight.descriptorSet.value())
        .setDstBinding(Bindings::SHADER_STORAGE)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eStorageBuffer)
        .setDescriptorCount(1)
        .setBufferInfo(storageBufferInfo);

    device.updateDescriptorSets(writeDescriptorSets, {});
}

void Vulkan::createSwapChainFrames()
{
    createInputTextures();
    std::vector<vk::Image> images;
    if(createInfo.windowEnabled())
    {
        images = swapChain.swapChain.value().getImages();
        if(swapChain.inFlightCount > images.size())
            throw std::runtime_error("Requested inflight count (N-buffering) is greater than number of swapchain images");
        if(swapChain.inFlightCount == 0)
            swapChain.inFlightCount = images.size();
    }
    else
        swapChain.inFlightCount = 1;
    vk::raii::CommandBuffers graphicsCommandBuffers(device, createInfo.commandBuffer(commandPools.graphics, swapChain.inFlightCount));
    vk::raii::CommandBuffers computeCommandBuffers(device, createInfo.commandBuffer(commandPools.compute, swapChain.inFlightCount));
    swapChain.descriptorPool.emplace(device, createInfo.descriptorPool(createInfo.inputImages().size(), createInfo.outputImages().size(), swapChain.inFlightCount));
    vk::raii::DescriptorSets descriptorSets(device, createInfo.descriptorSet(swapChain.descriptorPool.value(), swapChain.inFlightCount));
    swapChain.frames.clear();
    for(size_t i = 0; i < swapChain.inFlightCount; i++)
    {
        swapChain.frames.emplace_back();
        if(createInfo.windowEnabled())
            createInfo.createFrameBuffer(swapChain.frames.back(), images[i]);
        auto &inFlight = swapChain.inFlight.emplace_back();
        inFlight.commandBuffers.graphics.emplace(std::move(graphicsCommandBuffers[i]));
        inFlight.commandBuffers.compute.emplace(std::move(computeCommandBuffers[i]));
        createInfo.createFrameSync(inFlight);
        inFlight.buffers.uniform = memory.buffer(vk::BufferUsageFlagBits::eUniformBuffer, currentUniformBufferData.size());
        swapChain.inFlight.back().descriptorSet.emplace(std::move(descriptorSets[i]));
        auto outputImages = createInfo.outputImages();
        for(auto const &outputImage : outputImages)
           inFlight.outputTextures.emplace_back(memory.texture(outputImage, true)); 
        inFlight.buffers.shaderStorage = memory.buffer(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc, createInfo.shaderStorageBufferSize());
        updateShaderStorageBuffer(inFlight);
        updateDescriptorSets(inFlight);
    }
}

void Vulkan::recreateSwapChain()
{
    device.waitIdle();
    createInfo.updateResolution();
    oldSwapchain.emplace(std::move(swapChain.swapChain.value()));
    auto &swapChainCreateInfo = createInfo.swapChain();
    swapChainCreateInfo.setOldSwapchain(oldSwapchain.value());
    swapChain = Vulkan::SwapChain{device, swapChainCreateInfo, createInfo.windowEnabled()};
    createSwapChainFrames();
}

void Vulkan::CreateInfo::updateResolution()
{
    params.resolution = params.currentResolution();
}

void Vulkan::resize()
{
    resizeRequired = true;
}

void Vulkan::updateUniformBuffer(std::vector<uint32_t> buffer)
{
    std::copy(buffer.begin(), buffer.end(), currentUniformBufferData.begin());
}

int Vulkan::uniformIndex(std::string name) const
{
    int index = -1;
    for(size_t i = 0; i < uniformNames.size(); i++)
        if(uniformNames[i] == name)
        {
            index = i;
            break;
        }
    if(index == -1)
        std::cerr << "Could not find uniform " << name << std::endl;
    return index;
}

void Vulkan::updateUniform(std::string name, float value)
{
    int index = uniformIndex(name);
    if(index > -1)
        currentUniformBufferData[index] = *reinterpret_cast<uint32_t * >(&value);
}

void Vulkan::addToUniform(std::string name, float value)
{
    int index = uniformIndex(name);
    if(index > -1)
    {
        float uniformValue = *reinterpret_cast<float *>(&currentUniformBufferData[index]);
        uniformValue += value;
        currentUniformBufferData[index] = *reinterpret_cast<uint32_t * >(&uniformValue);
    }
}

void Vulkan::recordComputeCommandBuffer(SwapChain::InFlight &inFlight)
{
    auto &buffer = *inFlight.commandBuffers.compute;
    buffer.begin({});
    for(size_t i = 0; i < workGroupCounts.size(); i++)
    {
        buffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipelines.compute.pipelines[i]);
        buffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelines.compute.layout, 0, {inFlight.descriptorSet.value()}, {});
        buffer.dispatch(workGroupCounts[i].x, workGroupCounts[i].y, workGroupCounts[i].z);
        if(i < workGroupCounts.size() - 1)
        {
            vk::MemoryBarrier memoryBarrier;
            memoryBarrier
            .setSrcAccessMask(vk::AccessFlagBits::eShaderWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
            buffer.pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eComputeShader, {}, memoryBarrier, {}, {});
        }
    }
    buffer.end();
}

void Vulkan::compute(SwapChain::InFlight &inFlight)
{
    device.resetFences({*inFlight.fences.computeInFlight});
    recordComputeCommandBuffer(inFlight);

    vk::SubmitInfo submitInfo;
    submitInfo
    .setCommandBuffers({*inFlight.commandBuffers.compute.value()});
    if (createInfo.windowEnabled())
        submitInfo.setSignalSemaphores({*inFlight.semaphores.computeFinished.value()});
    queues.compute.submit({submitInfo}, *inFlight.fences.computeInFlight);
    
    computedInFlight++;
    if(computedInFlight == swapChain.inFlightCount)
    {
        computedInFlight = 0;
        workGroupCounts.clear();
    }
}

void Vulkan::computeSettings(std::vector<WorkGroupCount> shaderWorkGroupCounts, bool runBenchmark)
{
    workGroupCounts = shaderWorkGroupCounts;
    benchmark = runBenchmark;
}

std::shared_ptr<Loader::Image> Vulkan::resultTexture()
{
    const auto &inFlight = swapChain.lastComputedInFlight();
    auto &output = inFlight.outputTextures.front();
    auto initialOutputImage = createInfo.outputImages().front();

    auto stagingBuffer = memory.buffer(vk::BufferUsageFlagBits::eTransferDst, initialOutputImage->size());
    transitionImageLayout(output->image, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal);
    copyImageToBuffer(output->image, static_cast<vk::Buffer>(stagingBuffer->buffer), initialOutputImage->width(), initialOutputImage->height());

    void *gpuData;
    vmaMapMemory(*stagingBuffer->allocator, stagingBuffer->allocation, &gpuData);
    auto result = std::make_shared<Loader::ImageFfmpeg>(initialOutputImage->width(), initialOutputImage->height(), 1, initialOutputImage->imageFormat(), reinterpret_cast<uint8_t *>(gpuData));
    vmaUnmapMemory(*stagingBuffer->allocator, stagingBuffer->allocation);
    transitionImageLayout(output->image, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eGeneral);
    return result;
}

std::vector<float> Vulkan::resultBuffer()
{
    const auto &inFlight = swapChain.lastComputedInFlight();
    size_t size = createInfo.shaderStorageBufferSize();
    auto stagingBuffer = memory.buffer(vk::BufferUsageFlagBits::eTransferDst, size);
    std::vector<float> result(size);
    
    void *gpuData;
    copyBuffer(inFlight.buffers.shaderStorage->buffer, stagingBuffer->buffer, size);
    vmaMapMemory(*stagingBuffer->allocator, stagingBuffer->allocation, &gpuData);
    memcpy(result.data(), gpuData, size);
    vmaUnmapMemory(*stagingBuffer->allocator, stagingBuffer->allocation);    
    return result;
}

Vulkan::~Vulkan()
{
    device.waitIdle();
    swapChain.frames.clear();
}

