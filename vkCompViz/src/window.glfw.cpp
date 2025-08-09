module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ctype.h>
module window;
import std;
import common;
import glfwKeyStrings;

bool Window::Glfw::Keys::pressed(std::string name) const
{
    auto lowerName = name;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::toupper);
    lowerName.erase(std::remove(lowerName.begin(), lowerName.end(), ' '), lowerName.end());
    lowerName.erase(std::remove(lowerName.begin(), lowerName.end(), '_'), lowerName.end());
    return pressedKeys.contains(keyNames.find(lowerName)->second);
}

void Window::Glfw::keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    auto* self = static_cast<Glfw *>(glfwGetWindowUserPointer(window));
    if(action == GLFW_PRESS)
        self->keys.press(key);
    else if(action == GLFW_RELEASE)
        self->keys.release(key);
}

void Window::Glfw::resizeCallback(GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height)
{
    auto* self = static_cast<Glfw *>(glfwGetWindowUserPointer(window));
    self->setResized();
}

Window::Glfw::Glfw(const Parameters &parameters) :
    Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(parameters.resolution.width, parameters.resolution.height, parameters.title.c_str(), nullptr, nullptr);
    if(window == nullptr)
        throw std::runtime_error("Failed to create GLFW window");
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, Glfw::keyCallback);
    glfwSetFramebufferSizeCallback(window, Glfw::resizeCallback);
}

Resolution Window::Glfw::resolution() const
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

std::vector<const char *> Window::Glfw::requiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

uintptr_t Window::Glfw::getSurface(uintptr_t instance)
{
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(reinterpret_cast<VkInstance>(instance), window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");
    return reinterpret_cast<uintptr_t>(surface);
}

bool Window::Glfw::quit() const
{
    return glfwWindowShouldClose(window);
}

void Window::Glfw::run()
{
    glfwPollEvents();
}

Window::Glfw::~Glfw()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
