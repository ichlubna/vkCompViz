module window;
using namespace Window;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//import glm;

WindowGlfw::WindowGlfw(const Parameters &parameters) : Window(parameters)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(parameters.resolution.x, parameters.resolution.y, parameters.title.c_str(), nullptr, nullptr);
    if(window == nullptr)
        throw std::runtime_error("Failed to create GLFW window");
}
        
std::vector<const char*> WindowGlfw::requiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

void WindowGlfw::WindowGlfw::run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }
}

WindowGlfw::~WindowGlfw()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
