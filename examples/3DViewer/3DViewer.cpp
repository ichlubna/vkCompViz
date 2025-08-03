#include <cstdlib>
import vkCompViz;
import std;

int main()
{
    try
    {
        vkCompViz::App::Parameters params;
        // This vector contains the 3D model data
        // It consists of array of indices followed by vertices where each vertex is defined by 3D coordinates and 3D normal
        std::vector<float> inputData(10);
        // These data will be stored in storage buffer and used in vertex shader 
        params.shaders.storageBuffer.size = inputData.size() * sizeof(float);
        params.shaders.storageBuffer.initialData = inputData;
        // This overrides the default fullscreen display vertex and fragment shader
        params.shaders.vertex = "3DViewerVertex.slang";
        params.shaders.fragment = "3DViewerFragment.slang";
        // An empty compute shader is used, the library is mainly designed to work with compute shaders so at leats one is needed
        params.shaders.compute.push_back("3DViewerCompute.slang");
        params.shaders.workGroupCounts.push_back({1,1,1});
        // The viewer will support model rotation
        params.shaders.uniforms.push_back({"angle", 0.5f});
        // The angle can be adjusted either by pressing space and defining the new value
        // Or by pressing buttons to oncrease and decrease the angle by a step
        params.keyBindings.push_back({.uniform="angle", .keyIncrease="left", .keyDecrease="right", .step=0.01f});
        // This is a window app
        params.window.enable = true;
        params.window.resolution = {800, 600};
        params.window.title = "3D Viewer";
        // It is better to set constant low enough FPS when uniforms change interactively to maintain the same speed everywhere
        // Setting extreme values like 10000 leads to unlimited FPS and might end up at different values on different machines
        params.window.fps = 60.0f;
        vkCompViz::App app;
        app.run(params);       
        
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
