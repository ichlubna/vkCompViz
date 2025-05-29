#include <cstdlib>

import arguments;
import vkCompViz;
import std;


int main()
{
    vkCompViz::App app;
    try
    {
        //app.run({}) TODO test only compute;
        app.run(vkCompViz::App::ComputeParameters{},
                vkCompViz::App::WindowParameters{.width = 800, .height = 600, .title = "simpleBlending"});
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
