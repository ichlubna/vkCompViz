module;
#include <glm/glm.hpp>
export module window: interface;
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
        [[nodiscard]] virtual std::vector<const char *> requiredExtensions() const = 0;
        [[nodiscard]] virtual uintptr_t getSurface(uintptr_t vkInstance) = 0;
        [[nodiscard]] virtual bool key(std::string name) const = 0;
        [[nodiscard]] virtual bool quit() const = 0;
        [[nodiscard]] virtual glm::uvec2 resolution() const = 0;
        virtual ~Window() = default;
};
}
