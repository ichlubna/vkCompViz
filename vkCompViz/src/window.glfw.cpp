module;
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <ctype.h>
module window;
import std;
import common;
import glfwKeyStrings;

[[nodiscard]] std::string normalizeName(std::string name)
{
    auto normalizedName = name;
    transform(normalizedName.begin(), normalizedName.end(), normalizedName.begin(), ::toupper);
    normalizedName.erase(std::remove(normalizedName.begin(), normalizedName.end(), ' '), normalizedName.end());
    normalizedName.erase(std::remove(normalizedName.begin(), normalizedName.end(), '_'), normalizedName.end());
    return normalizedName;
}

bool Window::Glfw::Keys::pressed(std::string name) const
{
    auto normalizedName = normalizeName(name);
    return pressedKeys.contains(keyNames.find(normalizedName)->second);
}

bool Window::Glfw::Keys::mouse(std::string name)
{
    auto normalizedName = normalizeName(name);
    bool result = mouseActions.contains(mouseNames.find(normalizedName)->second);
    if(normalizedName == "MOUSESCROLL")
        mouseActionEnd(mouseNames.find("MOUSESCROLL")->second);
    return result;
}

void Window::Glfw::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto* self = static_cast<Glfw *>(glfwGetWindowUserPointer(window));
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    self->mouse.x = xpos / width;
    self->mouse.y = ypos / height;
}

void Window::Glfw::keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
    auto* self = static_cast<Glfw *>(glfwGetWindowUserPointer(window));
    if(action == GLFW_PRESS)
        self->keys.press(key);
    else if(action == GLFW_RELEASE)
        self->keys.release(key);
}

[[nodiscard]] std::pair<double, double> normalizedMousePosition(GLFWwindow *window)
{
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return {x / width, y / height};
}

void Window::Glfw::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* self = static_cast<Glfw *>(glfwGetWindowUserPointer(window));
    self->mouse.scrollX = xoffset;
    self->mouse.scrollY = yoffset;
    self->keys.mouseActionStart(GLFW_MOUSE_BUTTON_SCROLL);
}

void Window::Glfw::mouseButtonCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
{
    auto* self = static_cast<Glfw *>(glfwGetWindowUserPointer(window));
    if(action == GLFW_PRESS)
        self->keys.mouseActionStart(button);
    else if(action == GLFW_RELEASE)
        self->keys.mouseActionEnd(button);
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
    glfwSetScrollCallback(window, Glfw::scrollCallback);
    glfwSetMouseButtonCallback(window, Glfw::mouseButtonCallback);
    glfwSetFramebufferSizeCallback(window, Glfw::resizeCallback);
    glfwSetCursorPosCallback(window, Glfw::cursorPositionCallback);
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
