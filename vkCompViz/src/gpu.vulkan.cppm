module;
// TODO use glm, currently causing compiler error, probably caused by modules 
//#include <glm/glm.hpp>
export module gpu: vulkan;
export import : interface;
import std;
import vulkan_hpp;

export namespace Gpu
{
namespace glm
{
    class uvec2{public: unsigned int x; unsigned int y;};
}

class Vulkan : public Gpu
{
    public:
        class VulkanInitParams
        {
            public:
                std::vector<const char *> requiredExtensions;
                std::function<std::uintptr_t(std::uintptr_t)> surface;
                glm::uvec2 resolution;
                class ShaderCodes
                {
                    public:
                        std::vector<std::uint32_t> vertex;
                        std::vector<std::uint32_t> fragment;
                } shaderCodes;
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
                [[nodiscard]] vk::ApplicationInfo &application();
                [[nodiscard]] vk::InstanceCreateInfo &instance();
                [[nodiscard]] vk::raii::PhysicalDevice bestPhysicalDevice();
                [[nodiscard]] vk::DeviceCreateInfo &device();
                [[nodiscard]] std::vector<vk::DeviceQueueCreateInfo> &queues();
                [[nodiscard]] std::size_t graphicsQueueID() const
                {
                    return queueIndex.graphics;
                }
                [[nodiscard]] std::size_t computeQueueID() const
                {
                    return queueIndex.compute;
                }
                [[nodiscard]] std::size_t presentQueueID() const
                {
                    return queueIndex.present;
                }
                [[nodiscard]] vk::SwapchainCreateInfoKHR &swapChain(glm::uvec2 resolution);
                [[nodiscard]] vk::ShaderModuleCreateInfo &shaderModule(std::vector<std::uint32_t> &code);
                [[nodiscard]] vk::PipelineShaderStageCreateInfo &pipelineShaderStage(vk::raii::ShaderModule &shaderModule, vk::ShaderStageFlagBits stage);
                [[nodiscard]] vk::PipelineDynamicStateCreateInfo &pipelineDynamic();
                [[nodiscard]] vk::PipelineVertexInputStateCreateInfo &vertexInput();
                [[nodiscard]] vk::PipelineInputAssemblyStateCreateInfo &inputAssembly();
                [[nodiscard]] vk::PipelineViewportStateCreateInfo &viewport();
                [[nodiscard]] vk::PipelineRasterizationStateCreateInfo &rasterization();
                [[nodiscard]] vk::PipelineMultisampleStateCreateInfo &multisample(); 
                [[nodiscard]] vk::PipelineColorBlendAttachmentState &colorBlendAttachment();
                [[nodiscard]] vk::PipelineColorBlendStateCreateInfo &colorBlend();
                [[nodiscard]] vk::PipelineLayoutCreateInfo &pipelineLayout();
                [[nodiscard]] vk::RenderPassCreateInfo &renderPass();
                [[nodiscard]] vk::GraphicsPipelineCreateInfo &graphicsPipeline();

            private:
                Vulkan &vulkan;
                VulkanInitParams params;
                vk::ApplicationInfo applicationInfo{};
                vk::InstanceCreateInfo instanceCreateInfo{};
                vk::DeviceCreateInfo deviceCreateInfo{};
                vk::PhysicalDeviceFeatures physicalDeviceFeatures{};
                vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
                vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
                vk::PipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
                vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
                vk::PipelineViewportStateCreateInfo viewportCreateInfo{};
                vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
                vk::PipelineMultisampleStateCreateInfo multisampleCreateInfo{};
                vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{};
                vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
                vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
                vk::RenderPassCreateInfo renderPassCreateInfo{};
                vk::AttachmentDescription colorAttachment{};
                vk::AttachmentReference colorAttachmentReference{};
                vk::SubpassDescription subpass{};    
                vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
                vk::Viewport pipelineViewport{};
                vk::Rect2D pipelineScissor{};
                std::vector<vk::ShaderModuleCreateInfo> shaderModuleCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
                std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
                [[nodiscard]] std::vector<const char*> &allInstanceExtensions(); 
                std::vector<const char*> allExtensions;
                inline static const std::vector<const char*> instanceExtensions{"VK_KHR_portability_enumeration"};
                inline static const std::vector<const char*> validationLayers{"VK_LAYER_KHRONOS_validation"};
                inline static const std::vector<const char*> deviceExtensions{"VK_KHR_swapchain", "VK_KHR_shader_draw_parameters"};
                inline static const std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
                class QueueIndices
                {
                    public:
                    std::size_t graphics;
                    std::size_t compute;
                    std::size_t present;
                } queueIndex;
                inline static const float queuePriorities{1.0f};
        };
        std::unique_ptr<CreateInfo> createInfo;
        vk::raii::Context context;
        vk::raii::Instance instance;
        vk::raii::SurfaceKHR surface;
        vk::raii::PhysicalDevice physicalDevice;
        vk::raii::Device device;
        class Queues
        {
            public:
            vk::raii::Queue graphics;
            vk::raii::Queue compute;
            vk::raii::Queue present;
        } queues;
        class SwapChain
        {
            public:
            SwapChain(vk::raii::Device &device, const vk::SwapchainCreateInfoKHR &swapChainCreateInfo);
            vk::raii::SwapchainKHR swapChain;
            std::vector<vk::Image> images;
            std::vector<vk::raii::ImageView> imageViews;
            vk::Format imageFormat;
            vk::Extent2D extent;
        } swapChain;
        class Shaders
        {
            public:
            vk::raii::ShaderModule vertex;
            vk::raii::ShaderModule fragment; 
            std::vector<vk::raii::ShaderModule> compute;
        } shaders;
        class Pipelines
        {
            public:
            class Graphics
            {
                public:
                vk::raii::PipelineLayout layout;
                vk::raii::RenderPass renderPass;
                vk::raii::Pipeline pipeline;
            } graphics;
        } pipelines;
        void init() override;
};
}

