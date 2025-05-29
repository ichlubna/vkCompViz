module vkCompViz;

import std;
import gpu;
import window;

using namespace vkCompViz;

void App::run(ComputeParameters const &computeParameters, WindowParameters const &windowParameters)
{
    Window window;
    window.init({.width=windowParameters.width, .height=windowParameters.height, .title=windowParameters.title});
    window.run();
}

void App::run(ComputeParameters const &computeParameters)
{
}
