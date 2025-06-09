module;
#include <vulkan/vulkan.hpp>

module gpu;
import vulkan_hpp;
import common;
using namespace Gpu;

//import std;
Vulkan::Vulkan(VulkanInitParams params) :
    createInfo{std::make_unique<CreateInfo>(*this, params)},
    context{},
    instance{context, createInfo->instance()},
    surface{instance, reinterpret_cast<VkSurfaceKHR>(params.surface(reinterpret_cast<std::uintptr_t>(static_cast<VkInstance>(*instance))))},
    physicalDevice{createInfo->bestPhysicalDevice()},
    device{physicalDevice, createInfo->device()},
    graphicsQueue{device.getQueue(createInfo->graphicsQueueID(), 0)},
    computeQueue{device.getQueue(createInfo->computeQueueID(), 0)},
    presentQueue{device.getQueue(createInfo->presentQueueID(), 0)}
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
    extensions.insert(extensions.end(), defaultExtensions.begin(), defaultExtensions.end());

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

vk::raii::PhysicalDevice Vulkan::CreateInfo::bestPhysicalDevice()
{
    vk::raii::PhysicalDevices physicalDevices(vulkan.instance);
    if(physicalDevices.empty())
        throw std::runtime_error("No physical devices found");

    // This selection might be modified to allow for no-window application to run without graphics and presentation
    for(const auto& currentDevice : physicalDevices)
    {
        bool isDiscreteGpu = false;
        bool hasGraphicsQueue = false;
        bool hasComputeQueue = false;
        bool hasPresentQueue = false;

        const auto properties = currentDevice.getProperties();
        const auto queueFamilyProperties = currentDevice.getQueueFamilyProperties();
        for(auto [queueFamilyID, queueFamily] : queueFamilyProperties | std::ranges::views::enumerate)
        {
            if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                hasGraphicsQueue = true;
                graphicsQueueIndex = queueFamilyID;
            }
            if(queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
            {
                hasComputeQueue = true;
                computeQueueIndex = queueFamilyID;
            }
            if(currentDevice.getSurfaceSupportKHR(queueFamilyID, vulkan.surface))
            {
                hasPresentQueue = true;
                presentQueueIndex = queueFamilyID;
            }
        }
        if(properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            isDiscreteGpu = true;

        if(isDiscreteGpu && hasGraphicsQueue && hasComputeQueue && hasPresentQueue)
            return currentDevice;
    }
    throw std::runtime_error("No suitable GPU found");
}

std::vector<vk::DeviceQueueCreateInfo> &Vulkan::CreateInfo::queues()
{
    std::set<std::size_t> uniqueIDs = {graphicsQueueIndex, computeQueueIndex, presentQueueIndex};
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
    .setPEnabledFeatures(&physicalDeviceFeatures);
    return deviceCreateInfo;
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

