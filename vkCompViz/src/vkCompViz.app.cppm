module;
#include <glm/glm.hpp>

export module vkCompViz: app;
import std;

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
            glm::uvec2 resolution {1024, 720};
            std::string title {"No title"};
        };
        void run(ComputeParameters const &computeParameters, WindowParameters const &windowParameters);
        void run(ComputeParameters const &computeParameters);
    };

}
