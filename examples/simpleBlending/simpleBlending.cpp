#include <cstdlib>

import arguments;
import vkCompViz;
import std;


int main()
{
    try
    {
        vkCompViz::App app;
        app.useWindow({.resolution = {800, 600}, .title = "simpleBlending"});
        app.run(vkCompViz::App::ComputeParameters{});
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
