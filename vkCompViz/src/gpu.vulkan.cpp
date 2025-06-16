module;
#include <vulkan/vulkan.hpp>
module gpu;
import vulkan_hpp;
import common;
using namespace Gpu;

Vulkan::Vulkan(VulkanInitParams params) :
    createInfo{std::make_unique<CreateInfo>(*this, params)},
    context{},
    instance{context, createInfo->instance()},
    surface{instance, reinterpret_cast<VkSurfaceKHR>(params.surface(reinterpret_cast<std::uintptr_t>(static_cast<VkInstance>(*instance))))},
    physicalDevice{createInfo->bestPhysicalDevice()},
    device{physicalDevice, createInfo->device()},
    queues{ .graphics{device.getQueue(createInfo->graphicsQueueID(), 0)},
            .compute{device.getQueue(createInfo->computeQueueID(), 0)},
            .present{device.getQueue(createInfo->presentQueueID(), 0)}},
    swapChain{device, createInfo->swapChain(params.resolution)}
{
    init();
}

vk::InstanceCreateInfo &Vulkan::CreateInfo::instance()
{
    applicationInfo
    .setPApplicationName("My Vulkan App")
    .setPEngineName("My Engine")
    .setPNext(nullptr);

    extensions.insert(extensions.end(), params.requiredExtensions.begin(), params.requiredExtensions.end());
    extensions.insert(extensions.end(), instanceExtensions.begin(), instanceExtensions.end());

    instanceCreateInfo
    .setPApplicationInfo(&applicationInfo)
    .setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
    .setPEnabledExtensionNames(extensions);
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
    std::cout << "Capabilities: ";
    for(const auto& capability : uniqueCapabilities)
        std::cout << capability << " ";
    std::cout << std::endl;
    std::cout << "ID: " << id << std::endl; 
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
            std::cout << "____________" << std::endl;
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
        std::cout << "Selected as the most suitable: " << std::endl;
        bestDeviceRating.printInfo();
        std::cout << std::endl;
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

vk::Extent2D swapChainExtent(const vk::SurfaceCapabilitiesKHR &capabilities,glm::uvec2 resolution)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        return capabilities.currentExtent;
    vk::Extent2D extent = {resolution.x, resolution.y};    
    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
}

vk::SwapchainCreateInfoKHR &Vulkan::CreateInfo::swapChain(glm::uvec2 resolution)
{
    auto surfaceCapabilities = vulkan.physicalDevice.getSurfaceCapabilitiesKHR(vulkan.surface);
    auto surfaceFormat = swapChainSurfaceFormat(vulkan.physicalDevice.getSurfaceFormatsKHR(vulkan.surface));
    auto presentMode = swapChainPresentMode(vulkan.physicalDevice.getSurfacePresentModesKHR(vulkan.surface));
    auto extent = swapChainExtent(surfaceCapabilities,resolution);
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
    images = swapChain.getImages();
    vk::ImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo
        .setViewType(vk::ImageViewType::e2D)
        .setFormat(imageFormat)
        .setComponents({vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity})
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

    for (auto& image : images)
    {
        imageViewCreateInfo.setImage(image);
        imageViews.push_back(vk::raii::ImageView{device, imageViewCreateInfo});
    }
}
               
vk::ShaderModuleCreateInfo &Vulkan::CreateInfo::shaderModule(std::vector<uint32_t> &code)
{
    shaderModuleCreateInfo
    .setCodeSize(code.size() * sizeof(uint32_t))
    .setPCode(code.data());
    return shaderModuleCreateInfo;
}

void Vulkan::init()
{
}

void Vulkan::draw()
{

}

void Vulkan::compute()
{

}

Vulkan::~Vulkan()
{

}

