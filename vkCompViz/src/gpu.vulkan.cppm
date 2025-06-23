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
                std::function<Resolution()> currentResolution;
                Resolution resolution;
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
        void resize() override;
        ~Vulkan();

    private:
        class SwapChain
        {
            public:
            class Frame
            {
                public:
                vk::Image image;
                std::optional<vk::raii::ImageView> imageView;
                std::optional<vk::raii::Framebuffer> frameBuffer;
                std::optional<vk::raii::CommandBuffer> commandBuffer;
            };
            class InFlightSync
            {
                public: 
                class Sempahores
                {
                    public:
                    std::optional<vk::raii::Semaphore> imageAvailable;
                    std::optional<vk::raii::Semaphore> renderFinished;
                } semaphores;
                class Fences
                {
                    public:
                    std::optional<vk::raii::Fence> inFlight; 
                } fences; 
            };
            SwapChain(vk::raii::Device &device, const vk::SwapchainCreateInfoKHR &swapChainCreateInfo);
            vk::raii::SwapchainKHR swapChain;
            vk::Format imageFormat;
            vk::Extent2D extent;
            std::vector<Frame> frames;
            std::vector<InFlightSync> inFlightSyncs;
        };
        class CreateInfo
        {
            public:
                CreateInfo(Vulkan &vulkan, VulkanInitParams params) : vulkan(vulkan), params(params) {};
                void updateResolution();
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
                [[nodiscard]] vk::SwapchainCreateInfoKHR &swapChain();
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
                [[nodiscard]] vk::FramebufferCreateInfo &frameBuffer(vk::raii::ImageView &attachment);
                [[nodiscard]] vk::CommandPoolCreateInfo &commandPool(std::size_t queueFamilyID);
                [[nodiscard]] vk::CommandBufferAllocateInfo &commandBuffer(vk::raii::CommandPool &commandPool, std::uint32_t count);
                [[nodiscard]] vk::RenderPassBeginInfo &renderPassBegin(vk::raii::Framebuffer &frameBuffer);
                [[nodiscard]] vk::SemaphoreCreateInfo &semaphore();
                [[nodiscard]] vk::FenceCreateInfo &fence();
                [[nodiscard]] vk::ImageViewCreateInfo &imageView(vk::Format imageFormat);
                void createFrameBuffer(Vulkan::SwapChain::Frame &frame, vk::Image image);
                void createFrameSync(SwapChain::InFlightSync &frame);

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
                vk::FramebufferCreateInfo frameBufferCreateInfo{};
                vk::CommandPoolCreateInfo commandPoolCreateInfo{};
                vk::ImageViewCreateInfo imageViewCreateInfo{};
                vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
                vk::RenderPassBeginInfo renderPassBeginInfo{};
                vk::SemaphoreCreateInfo semaphoreCreateInfo{};
                vk::FenceCreateInfo fenceCreateInfo{};
                vk::AttachmentDescription colorAttachment{};
                vk::AttachmentReference colorAttachmentReference{};
                vk::SubpassDescription subpass{};
                vk::SubpassDependency subpassDependency{}; 
                vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
                vk::Viewport pipelineViewport{};
                vk::Rect2D pipelineScissor{};
                vk::ClearValue clearColor{{0.02f, 0.01f, 0.01f, 1.0f}};
                std::vector<vk::ShaderModuleCreateInfo> shaderModuleCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
                std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
                std::vector<vk::ImageView> frameBufferAttachments;
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
        CreateInfo createInfo;
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
        SwapChain swapChain;
        std::optional<vk::raii::SwapchainKHR> oldSwapchain;
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
        class CommandPools
        {
            public:
            vk::raii::CommandPool graphics;
            vk::raii::CommandPool compute;
        } commandPools;
        std::uint32_t currentFrameID{0};
        bool resizeRequired{false};
        void recordCommandBuffer(SwapChain::Frame &frame);
        void recreateSwapChain();
        void createSwapChainFrames();
        void init() override;
};
}

