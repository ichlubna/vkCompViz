module;
#include <memory>
#include <map>
export module vkCompViz: app;
import common;
import gpu;
import window;
import shader;

export namespace vkCompViz
{
class App
{
    public:
        class ComputeParameters
        {

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
};

}
