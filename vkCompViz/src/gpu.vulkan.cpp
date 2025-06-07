module gpu;
import vulkan_hpp;
using namespace Gpu;

//import std;
Vulkan::Vulkan(VulkanInitParams params) :
                    createInfo{*this, params},
                    context{},
                    instance{context, createInfo.instance()}
{
    init();
}

vk::InstanceCreateInfo Vulkan::CreateInfo::instance() const
{
    vk::ApplicationInfo appInfo = vk::ApplicationInfo{}
        .setPApplicationName("My Vulkan App")
        .setPEngineName("My Engine");

    vk::InstanceCreateInfo instanceCreateInfo = vk::InstanceCreateInfo{}
        .setPApplicationInfo(&appInfo)
        .setPEnabledExtensionNames(params.requiredExtensions);

    return instanceCreateInfo;
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

