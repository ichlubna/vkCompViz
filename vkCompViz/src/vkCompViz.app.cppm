export module vkCompViz: app;
import common;
import gpu;
import window;
import shader;
import std;

export namespace vkCompViz
{
/**
 * @brief Main application class for running compute and rendering pipelines.
 */
class App
{
    public:
        /**
         * @brief Parameters configuring the behavior of the App.
         */
        class Parameters
        {
            public:
                /**
                 * @brief Configuration for input and output textures.
                 */
                class Textures
                {
                    public:
                        /**
                         * @brief Configuration for an output texture.
                         */
                        class Output
                        {
                            public:
                                /** Resolution of the output texture (width x height). */
                                Resolution resolution{0, 0};

                                /** Whether to use floating point format for output texture. */
                                bool floatFormat{false};

                                /**
                                 * @brief Index of input texture whose resolution should be used.
                                 * Set to -1 to ignore.
                                 */
                                int sameResolutionAsInputID{-1};

                                /**
                                 * @brief Index of input texture whose format should be used.
                                 * Set to -1 to ignore.
                                 */
                                int sameFormatAsInputID{-1};
                        };

                        /** List of file paths to input textures. */
                        std::vector<std::string> input;

                        /** List of output texture configurations. */
                        std::vector<Output> output;
                } textures;

                /**
                 * @brief Configuration of shaders used in the application.
                 */
                class Shaders
                {
                    public:
                        /** List of compute shader file paths. The order defines execution order. */
                        std::vector<std::string> compute;

                        /** Work group counts corresponding to each compute shader. */
                        std::vector<Gpu::Gpu::WorkGroupCount> workGroupCounts;

                        /** Vertex shader file path. */
                        std::string vertex{"fullScreenVS.slang"};

                        /** Fragment shader file path. */
                        std::string fragment{"textureDisplayFS.slang"};

                        /** Number of vertices to be used for rendering (only relevant if custom vertex shader used). */
                        std::size_t vertexCount{3};

                        /** Number of iterations to run compute shaders (only in headless mode). */
                        std::size_t iterations{1};

                        /** List of uniform name-value pairs to be passed to shaders. */
                        std::vector<std::pair<std::string, float >> uniforms;

                        /**
                         * @brief Storage buffer configuration for shaders.
                         */
                        class StorageBuffer
                        {
                            public:
                                /** Size of the shader storage buffer in bytes. */
                                std::size_t size{4};

                                /** Initial data to fill the storage buffer with. */
                                std::vector<float> initialData;
                        } storageBuffer;
                } shaders;

                /**
                 * @brief Configuration for saving screenshots.
                 */
                class Screenshot
                {
                    public:
                        /** Directory path where screenshots are saved. */
                        std::string path{"./"};

                        /** File extension for screenshots. */
                        std::string extension{".png"};
                } screenshot;

                /**
                 * @brief Configuration for benchmark mode.
                 */
                class Benchmark
                {
                    public:
                        /** Enables or disables benchmark mode (headless only). */
                        bool enable{false};

                        /** Output path for benchmark data. */
                        std::string path{"./"};
                } benchmark;

                /**
                 * @brief Window configuration.
                 */
                class Window
                {
                    public:
                        /** Enables or disables application window. */
                        bool enable {true};

                        /** Resolution of the window (width x height). */
                        Resolution resolution {640, 480};

                        /** Title of the application window. */
                        std::string title {"No title"};

                        /** Frame rate of the window. */
                        float fps{60.0f};
                } window;

                /**
                 * @brief Defines a key binding to modify a uniform value via keyboard input.
                 */
                class KeyBinding
                {
                    public:
                        /** Name of the uniform variable to modify. */
                        std::string uniform;

                        /** Key that increases the uniform value. */
                        std::string keyIncrease;

                        /** Key that decreases the uniform value. */
                        std::string keyDecrease;

                        /** Step size for increasing or decreasing the uniform. */
                        float step{1.0f};
                };

                /** List of key bindings to modify uniforms when the window is enabled. */
                std::vector<KeyBinding> keyBindings;

                /** UUID of the GPU device to prioritize. */
                std::string priorityUUID{""};
        };

        /**
         * @brief Constructs a new App instance.
         */
        App();

        /**
         * @brief Runs the compute and rendering pipelines based on provided parameters.
         * @param inputParameters Parameters configuring the run.
         */
        void run(Parameters const &inputParameters);

        /**
         * @brief Retrieves the resolution of an image file.
         * @param path Path to the image file.
         * @return Resolution of the image (width x height).
         */
        [[nodiscard]] const Resolution getImageResolution(std::string path) const;

        /**
         * @brief Retrieves the work group size defined in a shader file.
         * @param path Path to the shader file.
         * @return WorkGroupSize structure with sizes in X, Y, Z.
         */
        [[nodiscard]] const Shader::Shader::Info::WorkGroupSize getShaderWorkGroupSize(std::string path) const;

        /**
         * @brief Calculates the number of work groups needed for given work group size and thread count.
         * @param workGroupSize Size of a work group.
         * @param threadCount Number of threads to dispatch.
         * @return WorkGroupCount indicating how many work groups are required.
         */
        [[nodiscard]] const Gpu::Gpu::WorkGroupCount calculateWorkGroupCount(Shader::Shader::Info::WorkGroupSize workGroupSize, Shader::Shader::Info::ThreadCount threadCount) const;

        /**
         * @brief Saves the first output texture to an image file.
         * @param path Path where the image should be saved.
         */
        void saveResultImage(std::string path) const;

        /**
         * @brief Retrieves the shader storage buffer result.
         * @param size Number of floats to retrieve; if 0, returns the whole buffer.
         * @return Vector of floats containing the result buffer.
         */
        std::vector<float> resultBuffer(std::size_t size = 0) const
        {
            return gpu->resultBuffer(size);
        };

        /**
         * @brief Returns a list of benchmark reports with performance data.
         * @return Vector of BenchmarkReport objects.
         */
        [[nodiscard]] const std::vector<Gpu::Gpu::BenchmarkReport> benchmarkReports() const
        {
            return gpu->benchmarkReports();
        }

        /**
         * @brief Destroys the App instance and cleans up resources.
         */
        ~App();

    private:
        Parameters parameters;
        std::unique_ptr<Window::Window> window;
        std::unique_ptr<Gpu::Gpu> gpu;
        std::unique_ptr<Shader::Shader> shader;
        Gpu::Vulkan::VulkanInitParams vulkanInitParams;
        void windowInit();
        void initShaders();
        void initTextures();
        void initUniforms() const;
        void initShaderStorageBuffer();
        void mainLoop();
};

}
