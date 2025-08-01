export module vkCompViz: app;
import common;
import gpu;
import window;
import shader;
import std;

export namespace vkCompViz
{
class App
{
    public:
        class Parameters
        {
            public:
                class Textures
                {
                    public:
                        class Output
                        {
                            public:
                                Resolution resolution{0, 0};
                                bool floatFormat{false};
                                int sameResolutionAsInputID{-1};
                                int sameFormatAsInputID{-1};
                        };
                        std::vector<std::string> input;
                        std::vector<Output> output;
                } textures;
                class Shaders
                {
                    public:
                        std::vector<std::string> compute;
                        std::vector<Gpu::Gpu::WorkGroupCount> workGroupCounts;
                        std::vector<std::pair<std::string, float >> uniforms;
                        class StorageBuffer
                        {
                            public:
                                std::size_t size{4};
                                std::vector<float> initialData;
                        } storageBuffer;
                } shaders;
                class Screenshot
                {
                    public:
                        std::string path{"./"};
                        std::string extension{".png"}; 
                } screenshot;
                class Benchmark
                {
                    public:
                        bool enable{false};
                        std::string path{"./"};
                } benchmark;
                class Window
                {
                    public:
                        bool enable {true};
                        Resolution resolution {640, 480};
                        std::string title {"No title"};
                } window;
        };
        class ParameterParser
        {
            private:
                std::string lastName{};
                std::map<std::string, float> parametersMap;
            public:
                void read();
                [[nodiscard]] float get(std::string name, float defaultValue = 0.0f) const;
                [[nodiscard]] const std::map<std::string, float> &get() const {return parametersMap;}
        };

        App();
        void run(Parameters const &inputParameters);
        [[nodiscard]] const Resolution getImageResolution(std::string path) const;
        [[nodiscard]] const Shader::Shader::Info::WorkGroupSize getShaderWorkGroupSize(std::string path) const;
        [[nodiscard]] const Gpu::Gpu::WorkGroupCount calculateWorkGroupCount(Shader::Shader::Info::WorkGroupSize workGroupSize, Shader::Shader::Info::ThreadCount threadCount) const;
        void saveResultImage(std::string path) const;
        std::vector<float> resultBuffer() const { return gpu->resultBuffer(); };
        [[nodiscard]] const std::vector<Gpu::Gpu::BenchmarkReport> benchmarkReports() const 
        {
            return gpu->benchmarkReports();
        }
        ~App();

    private:
        Parameters parameters;
        std::unique_ptr<Window::Window> window;
        std::unique_ptr<Gpu::Gpu> gpu;
        std::unique_ptr<Shader::Shader> shader;
        Gpu::Vulkan::VulkanInitParams vulkanInitParams;
        void windowInit();
        void initComputeShaders();
        void initTextures(); 
        void initUniforms() const;
        void initShaderStorageBuffer();
        void mainLoop();
};

}
