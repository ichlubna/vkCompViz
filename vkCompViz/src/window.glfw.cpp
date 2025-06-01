module;

#include <glm/glm.hpp>

module window;
using namespace Window;

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//import glm;

WindowGlfw::WindowGlfw(const Window::Parameters &parameters) : Window(parameters)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(parameters.width, parameters.height, parameters.title.c_str(), nullptr, nullptr);
    std::cerr << parameters.height << std::endl;
    if(window == nullptr)
        throw std::runtime_error("Failed to create GLFW window");
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
