module;
#include <bits/stdc++.h>
module window;
using namespace Window;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//import glm;

bool WindowGlfw::Keys::pressed(std::string name) const
{
    const std::unordered_map<std::string, int> keyNames
    {
        {"escape", GLFW_KEY_ESCAPE},
        {"enter", GLFW_KEY_ENTER},
        {"space", GLFW_KEY_SPACE}
    };
    auto lowerName = name;
    transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    return pressedKeys.contains(keyNames.find(lowerName)->second);
}

WindowGlfw::WindowGlfw(const Parameters &parameters) :
    Window(parameters)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(parameters.resolution.x, parameters.resolution.y, parameters.title.c_str(), nullptr, nullptr);
    if(window == nullptr)
        throw std::runtime_error("Failed to create GLFW window");
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, [](GLFWwindow * window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
    {
        auto* self = static_cast<WindowGlfw *>(glfwGetWindowUserPointer(window));
        if(action == GLFW_PRESS)
            self->keys.press(key);
        else if(action == GLFW_RELEASE)
            self->keys.release(key);
    });
}

std::vector<const char *> WindowGlfw::requiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

uintptr_t WindowGlfw::getSurface(uintptr_t instance)
{
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(reinterpret_cast<VkInstance>(instance), window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");
    return reinterpret_cast<uintptr_t>(surface);
}

bool WindowGlfw::quit() const
{
    return glfwWindowShouldClose(window);
}

void WindowGlfw::WindowGlfw::run()
{
    glfwPollEvents();
}

WindowGlfw::~WindowGlfw()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}
