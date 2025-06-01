module;
#include <glm/glm.hpp>

export module window:interface;
import std;

export namespace Window
{
    class Parameters
    {
        public:
        glm::uvec2 resolution;
        std::string title;        
    }; 

    class Window
    {
        public:
        Window(const Parameters &parameters) {};
        virtual void run() = 0;
        virtual ~Window() = default;
    };
}
