export module window: interface;
import common;
import std;

export namespace Window
{
class Parameters
{
    public:
        Resolution resolution;
        std::string title;
};

class Window
{
    public:
        Window(const Parameters &parameters) {};
        virtual void run() = 0;
        [[nodiscard]] virtual std::vector<const char *> requiredExtensions() const = 0;
        [[nodiscard]] virtual std::uintptr_t getSurface(std::uintptr_t vkInstance) = 0;
        [[nodiscard]] virtual bool key(std::string name) const = 0;
        [[nodiscard]] virtual bool resized() const = 0;
        [[nodiscard]] virtual bool quit() const = 0;
        [[nodiscard]] virtual Resolution resolution() const = 0;
        virtual ~Window() = default;
};
}
