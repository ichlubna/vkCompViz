export module window:glfw;

export import :interface;
import std;

export namespace Window
{
    class WindowGlfw : public Window
    {
        public:
        WindowGlfw(const Parameters &parameters);
        void run();
        ~WindowGlfw();

        private:
        typedef struct GLFWwindow GLFWwindow;
        GLFWwindow* window;
    };
}
