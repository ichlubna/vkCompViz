module;
#include <set>
export module window: glfw;
export import : interface;
import common;
import std;

export namespace Window
{
class Glfw : public Window
{
    public:
        typedef struct GLFWwindow GLFWwindow;
        Glfw(const Parameters &parameters);
        void run() override;
        [[nodiscard]] std::vector<const char *> requiredExtensions() const override;
        [[nodiscard]] std::uintptr_t getSurface(std::uintptr_t instance) override;
        [[nodiscard]] bool key(std::string name) const
        {
            return keys.pressed(name);
        }
        [[nodiscard]] bool quit() const override;
        [[nodiscard]] Resolution resolution() const override;
        void setResized()
        {
            wasResized = true;
        }
        bool resized() const override
        {
            return wasResized;
        }
        static void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods);
        static void resizeCallback(GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height);
        ~Glfw();

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
        GLFWwindow *window;
        bool wasResized{false};
};
}
