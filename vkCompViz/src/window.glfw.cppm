module;
#include <glm/glm.hpp>
#include <set>

export module window: glfw;
export import : interface;
import std;

export namespace Window
{
class WindowGlfw : public Window
{
    public:
        WindowGlfw(const Parameters &parameters);
        void run() override;
        [[nodiscard]] std::vector<const char *> requiredExtensions() const override;
        [[nodiscard]] uintptr_t getSurface(uintptr_t instance) override;
        [[nodiscard]] bool key(std::string name) const
        {
            return keys.pressed(name);
        }
        [[nodiscard]] bool quit() const;
        ~WindowGlfw();

    private:
        class Keys
        {
            public:
                [[nodiscard]] bool pressed(std::string name) const;
                void press(int code)
                {
                    pressedKeys.insert(code);
                }
                void release(int code)
                {
                    pressedKeys.erase(code);
                }

            private:
                std::set<int> pressedKeys;
        } keys;
        typedef struct GLFWwindow GLFWwindow;
        GLFWwindow *window;
};
}
