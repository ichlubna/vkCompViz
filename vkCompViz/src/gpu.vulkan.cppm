module;
#include <memory>
#include <vk_mem_alloc.h>
export module gpu: vulkan;
export import : interface;
import std;
import vulkan_hpp;
import common;
import shader;

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
                class Shaders
                {
                    public:
                        Shader::Shader::Info vertex;
                        Shader::Shader::Info fragment;
                        std::vector<Shader::Shader::Info> compute;
                        [[nodiscard]] size_t uniformBufferSize() const { return std::max(vertex.uniformBufferSize, fragment.uniformBufferSize); };
                        [[nodiscard]] size_t uniformBufferUint32Count() const { return std::ceil(uniformBufferSize()/sizeof(uint32_t)); };
                        [[nodiscard]] std::vector<std::string> uniformNames() const;
                } shaders;
                class Textures
                {
                    public:
                        std::vector<std::shared_ptr<Loader::Image>> input;
                        std::vector<std::shared_ptr<Loader::Image>> output;
                } textures;
        };
        Vulkan(VulkanInitParams params);
        void draw() override;
        void compute() override;
        void resize() override;
        void setInFlightFrames(std::size_t count) override;
        void updateUniformBuffer(std::vector<uint32_t> buffer) override;
        void updateUniform(std::string name, float value) override;
        ~Vulkan();

    private:
        class Buffer
        {
            public:
            VkBuffer buffer;
            VmaAllocation allocation;
            VmaAllocator *allocator;
            ~Buffer() { vmaDestroyBuffer(*allocator, buffer, allocation); }
        };
        class SwapChain
        {
            public:
            std::optional<vk::raii::DescriptorPool> descriptorPool;
            class Frame
            {
                public:
                vk::Image image;
                std::optional<vk::raii::ImageView> imageView;
                std::optional<vk::raii::Framebuffer> frameBuffer;
            };
            class InFlight
            {
                public: 
                std::optional<vk::raii::CommandBuffer> commandBuffer;
                std::unique_ptr<Buffer> uniformBuffer;
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
                std::optional<vk::raii::DescriptorSet> descriptorSet;
            };
            void nextInFlight() { inFlightID = (inFlightID + 1) % inFlightCount; };
            [[nodiscard]] InFlight &currentInFlight() { return inFlight[inFlightID]; };
            SwapChain(vk::raii::Device &device, const vk::SwapchainCreateInfoKHR &swapChainCreateInfo);
            vk::raii::SwapchainKHR swapChain;
            vk::Format imageFormat;
            vk::Extent2D extent;
            std::vector<Frame> frames;
            std::vector<InFlight> inFlight;
            std::uint32_t inFlightID{0};
            std::uint32_t inFlightCount{0};
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
                [[nodiscard]] vk::DescriptorSetLayoutCreateInfo &descriptorSetLayout();
                [[nodiscard]] vk::DescriptorPoolCreateInfo &descriptorPool(std::size_t count);
                [[nodiscard]] vk::DescriptorSetAllocateInfo &descriptorSet(vk::raii::DescriptorPool &descriptorPool, std::size_t count);
                void createFrameBuffer(Vulkan::SwapChain::Frame &frame, vk::Image image);
                void createFrameSync(SwapChain::InFlight &frame);

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
                vk::BufferCreateInfo bufferCreateInfo{};
                vk::AttachmentDescription colorAttachment{};
                vk::AttachmentReference colorAttachmentReference{};
                vk::SubpassDescription subpass{};
                vk::SubpassDependency subpassDependency{}; 
                vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
                vk::Viewport pipelineViewport{};
                vk::Rect2D pipelineScissor{};
                vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
                vk::DescriptorSetLayoutBinding binding{};
                vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};
                vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{};
                vk::ClearValue clearColor{{0.02f, 0.01f, 0.01f, 1.0f}};
                std::vector<vk::ShaderModuleCreateInfo> shaderModuleCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
                std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
                std::vector<vk::ImageView> frameBufferAttachments;
                std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
                std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
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
        class Memory
        {   
            public:
            Memory(vk::raii::Instance &instance, vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device);
            VmaAllocator allocator;
            [[nodiscard]] std::unique_ptr<Buffer> buffer(vk::BufferUsageFlags usage, size_t size);
            ~Memory() { vmaDestroyAllocator(allocator); }
        } memory;
        class Queues
        {
            public:
            vk::raii::Queue graphics;
            vk::raii::Queue compute;
            vk::raii::Queue present;
        } queues;
        class CommandPools
        {
            public:
            vk::raii::CommandPool graphics;
            vk::raii::CommandPool compute;
        } commandPools;
        vk::raii::DescriptorSetLayout descriptorSetLayout;
        SwapChain swapChain;
        std::optional<vk::raii::SwapchainKHR> oldSwapchain;
        std::vector<std::uint32_t> currentUniformBufferData;
        std::vector<std::string> uniformNames;
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
        class Texture
        {
            public:
            vk::raii::Image image;
            vk::raii::DeviceMemory memory;
            vk::raii::ImageView view;
            vk::raii::Sampler sampler;
            VmaAllocation allocation;
            VmaAllocator *allocator;
            ~Texture() { vmaDestroyImage(*allocator, *image, allocation); }
        };
        class Textures
        {
            public:
            std::vector<Texture> input;
            std::vector<Texture> output;
        } textures;
        bool resizeRequired{false};
        void recordCommandBuffer(SwapChain::Frame &frame, SwapChain::InFlight &inFlight);
        void recreateSwapChain();
        void createSwapChainFrames();
        void graphicsSubmit(std::size_t swapChainFrameID);
        void createAllocator();
        void updateUniformBuffer(SwapChain::InFlight &inFlight);
        void init() override;
};
}

