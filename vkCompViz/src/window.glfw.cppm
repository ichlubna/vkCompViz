export module window : glfw;

import std;

export class Window
{
    public:
    class Parameters
    {
        public:
        std::size_t width;
        std::size_t height;
        std::string title;        
    }; 
    void init(const Parameters &parameters);
    void run();
    void cleanup();

    private:
    typedef struct GLFWwindow GLFWwindow;
    GLFWwindow* window;
};
