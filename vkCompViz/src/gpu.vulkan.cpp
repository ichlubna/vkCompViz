module;
#include <vulkan/vulkan.hpp>
#define VMA_VULKAN_VERSION 1002000 
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
module gpu;
import vulkan_hpp;
using namespace Gpu;

Vulkan::Vulkan(VulkanInitParams params) :
    createInfo{*this, params},
    context{},
    instance{context, createInfo.instance()},
    surface{instance, reinterpret_cast<VkSurfaceKHR>(params.surface(reinterpret_cast<std::uintptr_t>(static_cast<VkInstance>(*instance))))},
    physicalDevice{createInfo.bestPhysicalDevice()},
    device{physicalDevice, createInfo.device()},
    memory{instance, physicalDevice, device},
    queues{ .graphics{device.getQueue(createInfo.graphicsQueueID(), 0)},
            .compute{device.getQueue(createInfo.computeQueueID(), 0)},
            .present{device.getQueue(createInfo.presentQueueID(), 0)}},
    commandPools{   .graphics{device, createInfo.commandPool(createInfo.graphicsQueueID())},
                    .compute{device, createInfo.commandPool(createInfo.computeQueueID())}},
    descriptorSetLayout{device, createInfo.descriptorSetLayout()},
    swapChain{device, createInfo.swapChain()},
    currentUniformBufferData{static_cast<std::uint32_t>(params.shaders.uniformBufferUint32Count()), 0},
    uniformNames{params.shaders.uniformNames()},
    shaders{.vertex{device, createInfo.shaderModule(params.shaders.vertex.code)},
            .fragment{device, createInfo.shaderModule(params.shaders.fragment.code)},
            .compute{}},
    pipelines{  .graphics{  .layout{device, createInfo.pipelineLayout()},
                            .renderPass{device, createInfo.renderPass()},
                            .pipeline{device, nullptr, createInfo.graphicsPipeline()}}}
{
    init();
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
    if (compute.size() == 0)
        return {};
    
    for (size_t i = 1; i < compute.size(); i++)
        if (!areVectorsSame(compute[0].uniformNames, compute[i].uniformNames))
            throw std::runtime_error("The uniform buffer does not contain the same member variables in all compute shaders");
    return compute[0].uniformNames;
}

size_t Vulkan::VulkanInitParams::Shaders::uniformBufferSize() const
{
    size_t size = 0;
    for (auto& computeShader : compute)
        size = std::max(computeShader.uniformBufferSize, size);
    return size;
}

vk::ApplicationInfo &Vulkan::CreateInfo::application()
{
    applicationInfo
    .setPApplicationName("My Vulkan App")
    .setPEngineName("My Engine")
    .setApiVersion(VK_API_VERSION_1_2)
    .setPNext(nullptr);
    return applicationInfo;
}

std::vector<const char*> &Vulkan::CreateInfo::allInstanceExtensions()
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
    DeviceRating(const vk::raii::PhysicalDevice *testedDevice, const vk::SurfaceKHR &testedSurface, const std::vector<const char*> &requiredExtensions);
    DeviceRating() = default;
    void printInfo();
    const vk::raii::PhysicalDevice *device;
    std::string name;
    std::string id;
    std::set<std::string> uniqueCapabilities;
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

DeviceRating::DeviceRating(const vk::raii::PhysicalDevice *testedDevice, const vk::SurfaceKHR &testedSurface, const std::vector<const char*> &requiredExtensions) : device{testedDevice}
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
        if(swapChainSupport(testedSurface))
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
        if(device->getSurfaceSupportKHR(queueFamilyID, testedSurface))
        {
            score++;
            uniqueCapabilities.insert("Present");
            queueIndex.present = queueFamilyID;
        }
    }

    for(size_t i=0; i<VK_UUID_SIZE; i++)
        id += std::to_string(properties.pipelineCacheUUID[i]);
}

vk::raii::PhysicalDevice Vulkan::CreateInfo::bestPhysicalDevice()
{
    vk::raii::PhysicalDevices physicalDevices(vulkan.instance);
    if(physicalDevices.empty())
        throw std::runtime_error("No physical devices found");

    vk::PhysicalDevice bestPhysicalDevice;
    DeviceRating bestDeviceRating;
    std::cout << "The following devices are available:" << std::endl;
    std::set<std::string> uniqueIDs;
    for(const auto& currentDevice : physicalDevices)
    {
        DeviceRating deviceRating{&currentDevice, vulkan.surface, deviceExtensions};
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
    deviceCreateInfo
    .setQueueCreateInfos(queues())
    .setPEnabledExtensionNames(deviceExtensions)
    .setPEnabledFeatures(&physicalDeviceFeatures);
    return deviceCreateInfo;
}

vk::SurfaceFormatKHR swapChainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) 
        return availableFormat; 
    return availableFormats.front();
}

vk::PresentModeKHR swapChainPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    if (availablePresentMode == vk::PresentModeKHR::eMailbox) 
        return availablePresentMode; 
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D swapChainExtent(const vk::SurfaceCapabilitiesKHR &capabilities, Resolution resolution)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        return capabilities.currentExtent;
    vk::Extent2D extent = {resolution.width, resolution.height};
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

vk::SwapchainCreateInfoKHR &Vulkan::CreateInfo::swapChain()
{
    auto surfaceCapabilities = vulkan.physicalDevice.getSurfaceCapabilitiesKHR(vulkan.surface);
    auto surfaceFormat = swapChainSurfaceFormat(vulkan.physicalDevice.getSurfaceFormatsKHR(vulkan.surface));
    auto presentMode = swapChainPresentMode(vulkan.physicalDevice.getSurfacePresentModesKHR(vulkan.surface));
    auto extent = swapChainExtent(surfaceCapabilities, params.resolution);
    size_t imageCount = surfaceCapabilities.minImageCount + 1;
    if(surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        imageCount = surfaceCapabilities.maxImageCount;

    swapChainCreateInfo
    .setSurface(vulkan.surface)
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

Vulkan::SwapChain::SwapChain(vk::raii::Device &device, const vk::SwapchainCreateInfoKHR &swapChainCreateInfo) : swapChain{device, swapChainCreateInfo}
{
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
    .setLayout(vulkan.pipelines.graphics.layout)
    .setRenderPass(vulkan.pipelines.graphics.renderPass)
    .setSubpass(0)
    .setBasePipelineHandle(VK_NULL_HANDLE)
    .setBasePipelineIndex(-1);
    return graphicsPipelineCreateInfo;
}

vk::FramebufferCreateInfo &Vulkan::CreateInfo::frameBuffer(vk::raii::ImageView &attachment)
{
    frameBufferAttachments = {attachment};
    frameBufferCreateInfo
    .setRenderPass(vulkan.pipelines.graphics.renderPass)
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
    .setRenderPass(vulkan.pipelines.graphics.renderPass)
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
                
vk::DescriptorSetLayoutCreateInfo &Vulkan::CreateInfo::descriptorSetLayout()
{
    binding
    .setBinding(0)
    .setDescriptorType(vk::DescriptorType::eUniformBuffer)
    .setDescriptorCount(1)
    .setStageFlags(vk::ShaderStageFlagBits::eCompute | vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);

    descriptorSetLayoutCreateInfo
    .setBindingCount(1)
    .setPBindings(&binding);
    return descriptorSetLayoutCreateInfo;
}

Vulkan::Memory::Memory(vk::raii::Instance &instance, vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device)
{
    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.instance = *instance;
    allocatorInfo.physicalDevice = *physicalDevice;
    allocatorInfo.device = *device;
    if(vmaCreateAllocator(&allocatorInfo, &allocator))
        throw std::runtime_error("Failed to create allocator");
}

void Vulkan::init()
{
    createSwapChainFrames();
}

void Vulkan::recordCommandBuffer(SwapChain::Frame &frame, SwapChain::InFlight &inFlight)
{
    auto &buffer = *inFlight.commandBuffer;
    buffer.begin({});
    buffer.beginRenderPass(createInfo.renderPassBegin(*frame.frameBuffer), vk::SubpassContents::eInline);
    buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.graphics.pipeline);
    buffer.setViewport(0, {vk::Viewport(0, 0, swapChain.extent.width, swapChain.extent.height, 0, 1)});
    buffer.setScissor(0, {vk::Rect2D({0, 0}, swapChain.extent)});
    buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelines.graphics.layout, 0, {inFlight.descriptorSet.value()}, {});
    buffer.draw(3, 1, 0, 0);
    buffer.endRenderPass();
    buffer.end();       
}

void Vulkan::graphicsSubmit(size_t swapChainFrameID)
{
    auto &inFlight = swapChain.currentInFlight();
    device.resetFences({*inFlight.fences.inFlight});
    recordCommandBuffer(swapChain.frames[swapChainFrameID], inFlight);

    std::vector<vk::PipelineStageFlags> waitStage{vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::SubmitInfo submitInfo;
    submitInfo
    .setCommandBuffers({*inFlight.commandBuffer.value()})
    .setSignalSemaphores({*inFlight.semaphores.renderFinished.value()})
    .setWaitSemaphores({*inFlight.semaphores.imageAvailable.value()})
    .setWaitDstStageMask(waitStage);

    queues.graphics.submit({submitInfo}, *inFlight.fences.inFlight); 
}

void Vulkan::updateUniformBuffer(SwapChain::InFlight &inFlight)
{
    vmaCopyMemoryToAllocation(*inFlight.uniformBuffer->allocator, currentUniformBufferData.data(), inFlight.uniformBuffer->allocation, 0, currentUniformBufferData.size()*sizeof(uint32_t));
}

void Vulkan::draw()
{     
    size_t timeout = std::numeric_limits<uint64_t>::max();
    auto &inFlight = swapChain.currentInFlight();
    while(device.waitForFences({inFlight.fences.inFlight.value()}, VK_TRUE, timeout) == vk::Result::eTimeout);
 
    uint32_t swapChainFrameID; 
    vk::Result result;
    try
    {
        std::tie(result, swapChainFrameID) = swapChain.swapChain.acquireNextImage(timeout, *inFlight.semaphores.imageAvailable);
    }
    catch (const vk::SystemError& err)
    {
        if (err.code() == vk::Result::eErrorOutOfDateKHR)
        {
            recreateSwapChain();
            return;
        }
        else
            throw;        
    }
    updateUniformBuffer(inFlight);
    graphicsSubmit(swapChainFrameID); 

    vk::PresentInfoKHR presentInfo;
    presentInfo
    .setWaitSemaphores({*inFlight.semaphores.renderFinished.value()})
    .setSwapchains({*swapChain.swapChain})
    .setPImageIndices(&swapChainFrameID);
    try
    {
        result = queues.present.presentKHR(presentInfo);
    }
    catch (const vk::SystemError& err)
    {
        if (err.code() == vk::Result::eErrorOutOfDateKHR || err.code() == vk::Result::eSuboptimalKHR)
        {
            recreateSwapChain();
            resizeRequired = false;
        }
        else
            throw;        
    }
    if (resizeRequired) 
    {
        recreateSwapChain();
        resizeRequired = false;
    }
    swapChain.nextInFlight();
}

vk::ImageViewCreateInfo &Vulkan::CreateInfo::imageView(vk::Format imageFormat)
{
    imageViewCreateInfo
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(imageFormat)
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
    frame.fences.inFlight = std::move(vk::raii::Fence(vulkan.device, fence()));
}

void Vulkan::CreateInfo::createFrameBuffer(Vulkan::SwapChain::Frame &frame, vk::Image image)
{
    frame.image = image;
    auto &imageViewCreateInfo = imageView(vulkan.swapChain.imageFormat);
    imageViewCreateInfo.setImage(image);
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

vk::DescriptorPoolCreateInfo &Vulkan::CreateInfo::descriptorPool(size_t count)
{
    descriptorPoolSizes.clear();
    descriptorPoolSizes.push_back({vk::DescriptorType::eUniformBuffer, static_cast<uint32_t>(count)});
    descriptorPoolCreateInfo
        .setMaxSets(count)
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
        .setPoolSizes(descriptorPoolSizes);
    return descriptorPoolCreateInfo;
}

vk::DescriptorSetAllocateInfo &Vulkan::CreateInfo::descriptorSet(vk::raii::DescriptorPool &descriptorPool, size_t count)
{
    descriptorSetLayouts.clear();
    for (size_t i = 0; i < count; i++)
        descriptorSetLayouts.push_back(vulkan.descriptorSetLayout);
    descriptorSetAllocateInfo
        .setDescriptorPool(descriptorPool)
        .setSetLayouts(descriptorSetLayouts);
    return descriptorSetAllocateInfo;
}

void Vulkan::createSwapChainFrames()
{
    auto images = swapChain.swapChain.getImages();
    if (swapChain.inFlightCount > images.size())
        throw std::runtime_error("Requested inflight count (N-buffering) is greater than number of swapchain images");
    if (swapChain.inFlightCount == 0)
        swapChain.inFlightCount = images.size();
    vk::raii::CommandBuffers commandBuffers(device, createInfo.commandBuffer(commandPools.graphics, swapChain.inFlightCount));
    swapChain.descriptorPool.emplace(device, createInfo.descriptorPool(swapChain.inFlightCount));
    vk::raii::DescriptorSets descriptorSets(device, createInfo.descriptorSet(swapChain.descriptorPool.value(), swapChain.inFlightCount));
    swapChain.frames.clear();
    size_t id = 0;
    for (auto& image : images)
    {
        swapChain.frames.emplace_back();
        createInfo.createFrameBuffer(swapChain.frames.back(), image);
        swapChain.inFlight.emplace_back();
        swapChain.inFlight.back().commandBuffer.emplace(std::move(commandBuffers[id]));
        createInfo.createFrameSync(swapChain.inFlight.back());
        swapChain.inFlight.back().uniformBuffer = memory.buffer(vk::BufferUsageFlagBits::eUniformBuffer, currentUniformBufferData.size());
        swapChain.inFlight.back().descriptorSet.emplace(std::move(descriptorSets[id]));
        
        vk::DescriptorBufferInfo bufferInfo;
        bufferInfo
            .setBuffer(swapChain.inFlight.back().uniformBuffer->buffer)
            .setOffset(0)
            .setRange(VK_WHOLE_SIZE);
        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet
            .setDstSet(*swapChain.inFlight.back().descriptorSet.value())
            .setDstBinding(0)
            .setDstArrayElement(0)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setDescriptorCount(1)
            .setBufferInfo(bufferInfo);
        device.updateDescriptorSets({writeDescriptorSet}, {});
        id++;
    }
}

void Vulkan::recreateSwapChain()
{
    device.waitIdle();
    createInfo.updateResolution();
    oldSwapchain.emplace(std::move(swapChain.swapChain));
    auto &swapChainCreateInfo = createInfo.swapChain();
    swapChainCreateInfo.setOldSwapchain(oldSwapchain.value());
    swapChain = Vulkan::SwapChain{device, swapChainCreateInfo};
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

void Vulkan::setInFlightFrames(std::size_t count)
{
    swapChain.inFlightCount = count;
    resize();
};

void Vulkan::updateUniformBuffer(std::vector<uint32_t> buffer)
{
    std::copy(buffer.begin(), buffer.end(), currentUniformBufferData.begin());
}

void Vulkan::updateUniform(std::string name, float value)
{
    int index = -1;
    for (size_t i = 0; i < uniformNames.size(); i++)
        if (uniformNames[i] == name)
        {
            index = i;
            break;
        }
    if (index > -1)
        currentUniformBufferData[index] = *reinterpret_cast<uint32_t*>(&value);
    else
        std::cerr << "Could not find uniform " << name << std::endl;
}

void Vulkan::compute()
{

}

Vulkan::~Vulkan()
{
    device.waitIdle();
    swapChain.frames.clear();
}

