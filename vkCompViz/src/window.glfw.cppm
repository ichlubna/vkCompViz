module;
#include <glm/glm.hpp>

export module window:glfw;
export import :interface;
import std;

export namespace Window
{
    class WindowGlfw : public Window
    {
        public:
        WindowGlfw(const Parameters &parameters);
        void run() override;
        [[nodiscard]] std::vector<const char*> requiredExtensions() const override;
        ~WindowGlfw();

        private:
        typedef struct GLFWwindow GLFWwindow;
        GLFWwindow* window;
    };
}
