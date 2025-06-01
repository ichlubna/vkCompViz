module vkCompViz;

import std;
import gpu;
import window;

using namespace vkCompViz;

void App::run(ComputeParameters const &computeParameters, WindowParameters const &windowParameters)
{
    std::unique_ptr<Window::Window> window = std::make_unique<Window::WindowGlfw>(Window::Parameters{.resolution=windowParameters.resolution, .title=windowParameters.title});
    window->run();
}

void App::run(ComputeParameters const &computeParameters)
{
}
