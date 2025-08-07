#include <cstdlib>
import arguments;
import vkCompViz;
import std;

int main(int argc, char *argv[])
{
    try
    {
        // Handling input parameters by Arguments library
        std::string help =  "This program blends two images together with a factor.\n"
                            "Usage: ./simpleBlending -i1 pathToFirstImage -i2 pathToSecondImage -f floatFactor -o pathToOutputImage\n";
        Arguments args(argc, argv);
        if(args.printHelpIfPresent(help))
            return EXIT_SUCCESS;
        if(!args["-i1"] || !args["-i2"] || !args["-f"] || !args["-o"])
            throw std::invalid_argument("Missing parameters");

        vkCompViz::App::Parameters params;
        // Order of the elements in the following vectors of resources matters (compute order, bound arrays of resources in shaders)
        // Compute pipeline is represented by a vector of shader source names
        // Shader names refer to Slang files in the shader directory in the example sources
        params.shaders.compute.push_back("blend.slang");
        // Input textures
        params.textures.input.push_back(args["-i1"]);
        params.textures.input.push_back(args["-i2"]);
        // Output textures where the first one is considered as the final result which is displayed and stored
        // Format and resolution can be set or copied from the input texture where the ID is its index in the input textures vector
        params.textures.output.push_back({.sameResolutionAsInputID = 0, .sameFormatAsInputID = 0});
        // The default uniform values can be set here
        params.shaders.uniforms.push_back({"factor", args["-f"]});
        // The output screenshots taken by F1 can be stored to a given directory with a given extension, the name is created from date and time
        auto outputPath = std::filesystem::path(args["-o"]).remove_filename().string();
        params.screenshot.path = outputPath;
        params.screenshot.extension = std::filesystem::path(args["-o"]).extension().string();
        // The benchmark report file taken by F2 will be stored to a given directory
        params.benchmark.path = outputPath;
        // Additionally, a number of compute iterations can be set for benchmark, the app would run draw given number of frames and end
        // If the number is non-zero, the app stores the report for each frame
        //params.benchmark.iterations = 5;

        // Init the application
        vkCompViz::App app;
        // Get the resolution of one of the input images
        auto resolution = app.getImageResolution(args["-i1"]);
        // Get the work group size of the compute shader
        auto workGroupSize = app.getShaderWorkGroupSize("blend.slang");
        // The number of workgroups for each compute shader is defined in this vector
        // The function inside is a helper to compute the necessary count of workgroups
        params.shaders.workGroupCounts = {app.calculateWorkGroupCount(workGroupSize, {resolution.width, resolution.height, 1})};

        // Tells the application to use a window with given properties
        params.window.enable = true;
        params.window.resolution = resolution;
        params.window.title = "Simple Blending";
        // Runs the rendering and computation
        app.run(params);
        // Storing the result screenshot at the end
        app.saveResultImage(args["-o"]);
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
