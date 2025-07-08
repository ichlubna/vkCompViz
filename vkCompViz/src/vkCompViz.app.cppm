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
        class ComputeParameters
        {
            public:
                class Textures
                {
                    public:
                        class Output
                        {
                            public:
                                std::string path{""};
                                Resolution resolution{0, 0};
                                bool floatFormat{false};
                                int sameResolutionAsInputID{-1};
                                int sameFormatAsInputID{-1};
                        };
                        std::vector<std::string> input;
                        std::vector<Output> output;
                } textures;
                std::vector<std::string> computeShaders;
                std::vector<std::pair<std::string, float >> uniforms;
        };
        class WindowParameters
        {
            public:
                Resolution resolution {1920, 1080};
                std::string title {"No title"};
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
        void useWindow(Window::Parameters const &windowParameters);
        void run(ComputeParameters const &computeParameters);
        ~App();

    private:
        std::unique_ptr<Window::Window> window;
        std::unique_ptr<Gpu::Gpu> gpu;
        std::unique_ptr<Shader::Shader> shader;
        Gpu::Vulkan::VulkanInitParams vulkanInitParams;
        void windowInitParams();
        void initComputeShaders(ComputeParameters const &computeParameters);
        void initTextures(ComputeParameters const &computeParameters); 
        void initUniforms(ComputeParameters const &computeParameters) const;
        void mainLoop();
};

}
