export module gpu:vulkan;
export import :interface;
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
            std::vector<const char*> requiredExtensions;
        };
        Vulkan(VulkanInitParams params);
        void draw() override;
        void compute() override;
        ~Vulkan();

        private:
        class CreateInfo
        {
            public:
            CreateInfo(Vulkan &vulkan, VulkanInitParams params) : vulkan(vulkan), params(params){};
            [[nodiscard]] vk::InstanceCreateInfo instance() const;

            private:
            Vulkan &vulkan;
            VulkanInitParams params;
        } createInfo;
        vk::raii::Context context;
        vk::raii::Instance instance;
        void init() override;
    };
}

