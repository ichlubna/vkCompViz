#include <cstdlib>
import arguments;
import vkCompViz;
import std;

int main(int argc, char *argv[])
{
    try
    {
        std::string help =  "This program blends two images together with a factor.\n"
                            "Usage: ./simpleBlending -i1 pathToFirstImage -i2 pathToSecondImage -f floatFactor -o pathToOutputImage\n";
        Arguments args(argc, argv);
        if(args.printHelpIfPresent(help))
            return EXIT_SUCCESS;

        if(!args["-i1"] || !args["-i2"] || !args["-f"] || !args["-o"])
            throw std::invalid_argument("Missing parameters");

        vkCompViz::App::ComputeParameters params;
        params.computeShaders.push_back("blend.slang");
        params.textures.input.push_back(args["-i1"]);
        params.textures.input.push_back(args["-i2"]);
        params.textures.output.push_back({.path = args["-o"], .sameResolutionAsInputID = 0, .sameFormatAsInputID = 0});
        params.uniforms.push_back({"factor", args["-f"]});

        vkCompViz::App app;
        auto resolution = app.getImageResolution(args["-i1"]);
        auto workGroupSize = app.getShaderWorkGroupSize("blend.slang");
        params.workGroupCounts = {app.calculateWorkGroupCount(workGroupSize, {resolution.width, resolution.height, 1})};
        app.useWindow({.resolution = resolution, .title = "simpleBlending"});
        app.run(params);
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
