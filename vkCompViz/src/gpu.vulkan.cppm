export module gpu: vulkan;
export import : interface;
import std;
import vulkan_hpp;

export namespace Gpu
{
class Vulkan : public Gpu
{
    public:
        class VulkanInitParams
        {
            public:
                std::vector<const char *> requiredExtensions;
                std::function<std::uintptr_t(std::uintptr_t)> surface;
        };
        Vulkan(VulkanInitParams params);
        void draw() override;
        void compute() override;
        ~Vulkan();

    private:
        class CreateInfo
        {
            public:
                CreateInfo(Vulkan &vulkan, VulkanInitParams params) : vulkan(vulkan), params(params) {};
                [[nodiscard]] vk::InstanceCreateInfo &instance();
                [[nodiscard]] vk::raii::PhysicalDevice bestPhysicalDevice();
                [[nodiscard]] vk::DeviceCreateInfo &device();
                [[nodiscard]] std::vector<vk::DeviceQueueCreateInfo> &queues();
                [[nodiscard]] std::size_t graphicsQueueID() const
                {
                    return graphicsQueueIndex;
                }
                [[nodiscard]] std::size_t computeQueueID() const
                {
                    return computeQueueIndex;
                }
                [[nodiscard]] std::size_t presentQueueID() const
                {
                    return presentQueueIndex;
                }

            private:
                Vulkan &vulkan;
                VulkanInitParams params;
                vk::ApplicationInfo applicationInfo{};
                vk::InstanceCreateInfo instanceCreateInfo{};
                vk::DeviceCreateInfo deviceCreateInfo{};
                vk::PhysicalDeviceFeatures physicalDeviceFeatures{};
                std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
                std::vector<const char *> extensions;
                inline static const std::vector<const char *> defaultExtensions{"VK_KHR_portability_enumeration"};
                inline static const std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};
                std::size_t graphicsQueueIndex;
                std::size_t computeQueueIndex;
                std::size_t presentQueueIndex;
                inline static const float queuePriorities{1.0f};
        };
        std::unique_ptr<CreateInfo> createInfo;
        vk::raii::Context context;
        vk::raii::Instance instance;
        vk::raii::SurfaceKHR surface;
        vk::raii::PhysicalDevice physicalDevice;
        vk::raii::Device device;
        vk::raii::Queue graphicsQueue;
        vk::raii::Queue computeQueue;
        vk::raii::Queue presentQueue;
        void init() override;
};
}

