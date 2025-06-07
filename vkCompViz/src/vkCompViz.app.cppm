module;
#include <memory>
#include <glm/glm.hpp>

export module vkCompViz: app;
import gpu;
import window;
//import std;

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
            glm::uvec2 resolution {1920, 1080};
            std::string title {"No title"};
        };
        
        App();
        void useWindow(Window::Parameters const &windowParameters);
        void run(ComputeParameters const &computeParameters);
        ~App();

        private:
        std::unique_ptr<Window::Window> window;
        std::unique_ptr<Gpu::Gpu> gpu;
    };

}
