module;
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
                bool window {true};
                Resolution resolution;
                std::string deviceUUID{""};
                class Shaders
                {
                    public:
                        Shader::Shader::Info vertex;
                        Shader::Shader::Info fragment;
                        std::size_t vertexCount{3};
                        std::vector<Shader::Shader::Info> compute;
                        [[nodiscard]] size_t uniformBufferSize() const;
                        [[nodiscard]] size_t uniformBufferUint32Count() const
                        {
                            return std::ceil(uniformBufferSize() / sizeof(uint32_t));
                        };
                        [[nodiscard]] std::vector<std::string> uniformNames() const;
                } shaders;
                class Textures
                {
                    public:
                        std::vector<std::shared_ptr<Loader::Image >> input;
                        std::vector<std::shared_ptr<Loader::Image >> output;
                } textures;
                class ShaderStorageBuffer
                {
                    public:
                        std::size_t size;
                        std::vector<float> initialData;
                } shaderStorageBuffer;
        };
        Vulkan(VulkanInitParams params);
        void run() override;
        void computeSettings(std::vector<WorkGroupCount> shaderWorkGroupCounts, bool runBenchmark) override;
        void resize() override;
        void updateUniformBuffer(std::vector<uint32_t> buffer) override;
        void updateUniform(std::string name, float value) override;
        void addToUniform(std::string name, float value) override;
        void printUniforms() const override;
        [[nodiscard]] std::shared_ptr<Loader::Image> resultTexture() override;
        [[nodiscard]] std::vector<float> resultBuffer(size_t size = 0) override;
        ~Vulkan();

    private:
        class Buffer
        {
            public:
                VkBuffer buffer;
                VmaAllocation allocation;
                VmaAllocator *allocator;
                ~Buffer()
                {
                    vmaDestroyBuffer(*allocator, buffer, allocation);
                }
        };
        class Texture
        {
            public:
                VkImage image;
                VmaAllocation allocation;
                VmaAllocator *allocator;
                std::optional<vk::raii::ImageView> view;
                ~Texture()
                {
                    vmaDestroyImage(*allocator, image, allocation);
                }
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
                        std::unique_ptr<Texture> depth;
                };
                class InFlight
                {
                    public:
                        class CommandBuffers
                        {
                            public:
                                std::optional<vk::raii::CommandBuffer> graphics;
                                std::optional<vk::raii::CommandBuffer> compute;
                        } commandBuffers;
                        std::optional<vk::raii::DescriptorSet> descriptorSet;
                        std::vector<std::unique_ptr<Texture >> outputTextures;
                        class Buffers
                        {
                            public:
                                std::unique_ptr<Buffer> uniform;
                                std::unique_ptr<Buffer> shaderStorage;
                        } buffers;
                        class Sempahores
                        {
                            public:
                                std::optional<vk::raii::Semaphore> imageAvailable;
                                std::optional<vk::raii::Semaphore> renderFinished;
                                std::optional<vk::raii::Semaphore> computeFinished;
                        } semaphores;
                        class Fences
                        {
                            public:
                                std::optional<vk::raii::Fence> inFlight;
                                std::optional<vk::raii::Fence> computeInFlight;
                        } fences;
                        class QueryPools
                        {
                            public:
                                std::optional<vk::raii::QueryPool> compute;
                                size_t computeCount{0};
                                std::optional<vk::raii::QueryPool> graphics;
                        } queryPools;
                };
                void nextInFlight()
                {
                    previousInFlightID = inFlightID;
                    inFlightID = (inFlightID + 1) % inFlightCount;
                };
                [[nodiscard]] InFlight &currentInFlight()
                {
                    return inFlight[inFlightID];
                };
                [[nodiscard]] InFlight &lastComputedInFlight()
                {
                    if(previousInFlightID < 0)
                        throw std::runtime_error("No frames were computed yet");
                    return inFlight[previousInFlightID];
                }
                std::optional<vk::raii::SwapchainKHR> swapChain;
                std::optional<vk::raii::SwapchainKHR> oldSwapChain;
                vk::Format imageFormat;
                vk::Extent2D extent;
                std::vector<Frame> frames;
                std::vector<InFlight> inFlight;
                std::uint32_t inFlightID{0};
                std::uint32_t inFlightCount{0};
                int previousInFlightID{-1};
        };
        class CreateInfo
        {
            public:
                enum ImageType { Depth = 0, Storage = 1, Read = 2 };
                CreateInfo(Vulkan &vulkan, VulkanInitParams params) : vulkan(vulkan), params(params) {};
                void updateResolution();
                [[nodiscard]] Resolution currentResolution() const
                {
                    return params.resolution;
                }
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
                [[nodiscard]] vk::ComputePipelineCreateInfo &computePipeline(vk::raii::ShaderModule &shaderModule, vk::raii::PipelineLayout &pipelineLayout);
                [[nodiscard]] vk::FramebufferCreateInfo &frameBuffer(vk::raii::ImageView &color, vk::raii::ImageView &depth);
                [[nodiscard]] vk::CommandPoolCreateInfo &commandPool(std::size_t queueFamilyID);
                [[nodiscard]] vk::CommandBufferAllocateInfo &commandBuffer(vk::raii::CommandPool &commandPool, std::uint32_t count);
                [[nodiscard]] vk::RenderPassBeginInfo &renderPassBegin(vk::raii::Framebuffer &frameBuffer);
                [[nodiscard]] vk::SemaphoreCreateInfo &semaphore();
                [[nodiscard]] vk::FenceCreateInfo &fence();
                [[nodiscard]] vk::ImageViewCreateInfo &imageView(vk::Format imageFormat, vk::Image image, bool depth = false);
                [[nodiscard]] vk::ImageCreateInfo &image(vk::Format imageFormat, Resolution resolution, ImageType imageType);
                [[nodiscard]] vk::DescriptorSetLayoutCreateInfo &descriptorSetLayout(size_t inputTextureCount, size_t outputTextureCount);
                [[nodiscard]] vk::DescriptorPoolCreateInfo &descriptorPool(size_t inputTextureCount, size_t outputTextureCount, size_t inFlightFramesCount);
                [[nodiscard]] vk::DescriptorSetAllocateInfo &descriptorSet(vk::raii::DescriptorPool &descriptorPool, std::size_t count);
                [[nodiscard]] vk::SamplerCreateInfo &sampler();
                void createFrameBuffer(Vulkan::SwapChain::Frame &frame, vk::Image image);
                void createFrameSync(SwapChain::InFlight &frame);
                [[nodiscard]] const std::vector<std::shared_ptr<Loader::Image >> &outputImages() const { return params.textures.output; }
                [[nodiscard]] const std::vector<std::shared_ptr<Loader::Image >> &inputImages() const
                {
                    return params.textures.input;
                }
                [[nodiscard]] std::size_t shaderStorageBufferSize() const
                {
                    return params.shaderStorageBuffer.size;
                }
                [[nodiscard]] const std::vector<float> &shaderStorageBufferData() const
                {
                    return params.shaderStorageBuffer.initialData;
                }
                [[nodiscard]] VkSurfaceKHR surface();
                [[nodiscard]] bool windowEnabled() const
                {
                    return params.window;
                }
                [[nodiscard]] std::size_t vertexCount() const
                {
                    return params.shaders.vertexCount;
                }
                [[nodiscard]] std::size_t computeShaderCount() const
                {
                    return params.shaders.compute.size();
                }

            private:
                Vulkan &vulkan;
                VulkanInitParams params;
                vk::ApplicationInfo applicationInfo{};
                vk::InstanceCreateInfo instanceCreateInfo{};
                vk::DeviceCreateInfo deviceCreateInfo{};
                vk::PhysicalDeviceFeatures physicalDeviceFeatures{};
                vk::PhysicalDeviceVulkan12Features physicalDeviceVulkan12Features{};
                vk::PhysicalDeviceComputeShaderDerivativesFeaturesKHR physicalDeviceComputeShaderDerivativesFeatures{};
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
                vk::ImageCreateInfo imageCreateInfo{};
                vk::AttachmentDescription colorAttachment{};
                vk::AttachmentDescription depthAttachment{};
                vk::AttachmentReference colorAttachmentReference{};
                vk::AttachmentReference depthAttachmentReference{};
                vk::SubpassDescription subpass{};
                vk::SubpassDependency subpassDependency{};
                vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};
                vk::ComputePipelineCreateInfo computePipelineCreateInfo{};
                vk::Viewport pipelineViewport{};
                vk::Rect2D pipelineScissor{};
                vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
                vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};
                vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{};
                vk::SamplerCreateInfo samplerCreateInfo{};
                vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
                std::vector<vk::ClearValue> clearColors;
                std::vector<vk::AttachmentDescription> attachments = {};
                vk::PipelineShaderStageCreateInfo shaderStage{};
                std::vector<vk::ShaderModuleCreateInfo> shaderModuleCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
                std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
                std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
                std::vector<vk::ImageView> frameBufferAttachments;
                std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
                std::vector<vk::DescriptorSetLayoutBinding> bindings{};
                std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
                std::vector<uint32_t> queueFamilyIndices;
                [[nodiscard]] std::vector<const char *> &allInstanceExtensions();
                std::vector<const char *> allExtensions;
                inline static const std::vector<const char *> instanceExtensions{"VK_KHR_portability_enumeration"};
                inline static const std::vector<const char *> validationLayers{"VK_LAYER_KHRONOS_validation"};
                std::vector<const char *> deviceExtensions{"VK_KHR_shader_draw_parameters", "VK_KHR_compute_shader_derivatives"};
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
        std::optional<vk::raii::SurfaceKHR> surface;
        vk::raii::PhysicalDevice physicalDevice;
        vk::raii::Device device;
        class Memory
        {
            public:
                Memory(vk::raii::Instance &instance, vk::raii::PhysicalDevice &physicalDevice, vk::raii::Device &device, Vulkan &vulkan);
                VmaAllocator allocator;
                [[nodiscard]] std::unique_ptr<Buffer> buffer(vk::BufferUsageFlags usage, size_t size);
                [[nodiscard]] std::unique_ptr<Texture> texture(std::shared_ptr<Loader::Image> image, bool storage = false);
                [[nodiscard]] std::unique_ptr<Texture> depth(Resolution resolution);
                ~Memory()
                {
                    vmaDestroyAllocator(allocator);
                }
            private:
                Vulkan &vulkan;
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
        vk::raii::Sampler sampler;
        vk::raii::DescriptorSetLayout descriptorSetLayout;
        SwapChain swapChain;
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
                        Graphics() = default;
                        std::optional<vk::raii::PipelineLayout> layout;
                        std::optional<vk::raii::RenderPass> renderPass;
                        std::optional<vk::raii::Pipeline> pipeline;
                } graphics;
                class Compute
                {
                    public:
                        Compute() = default;
                        Compute(std::vector<vk::raii::ShaderModule> &shaders, vk::raii::Device &device, CreateInfo &createInfo);
                        std::optional<vk::raii::PipelineLayout> layout;
                        std::vector<vk::raii::Pipeline> pipelines;
                } compute;
        } pipelines;
        class OneTimeCommand
        {
            public:
                vk::raii::CommandBuffer &command()
                {
                    return buffer.value();
                }
                std::optional<vk::raii::CommandBuffer> buffer;
                vk::raii::Queue *queue;
                ~OneTimeCommand();
        };
        class Bindings
        {
            public:
                static constexpr size_t UNIFORM{0};
                static constexpr size_t OUTPUT_TEXTURE_SAMPLER{1};
                static constexpr size_t OUTPUT_TEXTURE_STORAGE{2};
                static constexpr size_t INPUT_TEXTURE_SAMPLER{3};
                static constexpr size_t INPUT_TEXTURE{4};
                static constexpr size_t SHADER_STORAGE{5};
        };
        class Times
        {
            public:
                class Memory
                {
                    public:
                        class Download
                        {
                            public:
                                float texture{0};
                                float shaderStorage{0};
                        } download;
                        class Upload
                        {
                            public:
                                float texture{0};
                                float shaderStorage{0};
                        } upload;
                } memory;
        } times;
        std::vector<std::unique_ptr<Texture >> inputTextures;
        std::vector<WorkGroupCount> workGroupCounts;
        const size_t timeout = std::numeric_limits<uint64_t>::max();
        size_t computedInFlight{0};
        bool resizeRequired{false};
        void initSwapChain();
        void createComputePipeline();
        void recordGraphicsCommandBuffer(SwapChain::Frame &frame, SwapChain::InFlight &inFlight);
        void recordComputeCommandBuffer(SwapChain::InFlight &inFlight);
        void recreateSwapChain();
        void createSwapChainFrames();
        void graphicsSubmit(std::size_t swapChainFrameID);
        void compute(SwapChain::InFlight &inFlight);
        void draw(SwapChain::InFlight &inFlight);
        void createAllocator();
        void updateUniformBuffer(SwapChain::InFlight &inFlight);
        void updateShaderStorageBuffer(SwapChain::InFlight &inFlight);
        void copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size);
        void transitionImageLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
        void copyBufferToImage(vk::Buffer buffer, vk::Image image, size_t width, size_t height);
        void copyImageToBuffer(vk::Image image, vk::Buffer outputBuffer, size_t width, size_t height);
        void updateDescriptorSets(SwapChain::InFlight &inFlight);
        void createInputTextures();
        void newBenchmark(SwapChain::InFlight &inFlight);
        void updateBenchmarks();
        void createQueryPools(SwapChain::InFlight &inFlight);
        void waitForGraphics(SwapChain::InFlight &inFlight);
        void waitForCompute(SwapChain::InFlight &inFlight);
        [[nodiscard]] int uniformIndex(std::string name) const;
        std::unique_ptr<OneTimeCommand> oneTimeCommand();
        [[nodiscard]] std::vector<vk::raii::ShaderModule> createComputeShaders(std::vector<Shader::Shader::Info> &computeShaders);
        void init() override;
};
}

