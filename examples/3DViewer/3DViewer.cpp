#include <cstdlib>
import vkCompViz;
import objLoader;
import std;

int main(int argc, char *argv[])
{
    try
    {
        if(argc < 2)
            throw std::invalid_argument("Missing input obj file (containing normals and triangulated faces)");
        // This simple library loads OBJ 3D files
        ObjLoader objLoader(argv[1]);
        // The vertices are nornalized so that any object fits the window
        objLoader.normalizeVertices();

        vkCompViz::App::Parameters params;
        // This vector contains the 3D model data
        // It consists of array of indices followed by and array of 3D coordinates of vertices and and another array of normals
        std::vector<float> inputData = objLoader.data();
        // These data will be stored in storage buffer and used in vertex shader
        params.shaders.storageBuffer.size = inputData.size() * sizeof(float);
        params.shaders.storageBuffer.initialData = inputData;
        // This overrides the default fullscreen display vertex and fragment shader
        params.shaders.vertex = "3DViewerVertex.slang";
        params.shaders.fragment = "3DViewerFragment.slang";
        // This overrides the default vertex count of 3 for fullscreen triangle
        params.shaders.vertexCount = objLoader.indices.size() / 2;
        // An empty compute shader is used, the library is mainly designed to work with compute shaders so at leats one is needed
        // The compute shader could, for example, edit or generate the geometry here
        params.shaders.compute.push_back("3DViewerCompute.slang");
        params.shaders.workGroupCounts.push_back({1, 1, 1});
        // The viewer will support model rotation
        params.shaders.uniforms.push_back({"angle", 180.0f});
        // The following uniforms are indices to the buffer
        params.shaders.uniforms.push_back({"verticesStart", objLoader.indices.size()});
        params.shaders.uniforms.push_back({"normalsStart", objLoader.indices.size() + objLoader.vertices.size()});
        // The angle can be adjusted either by pressing space and defining the new value
        // Or by pressing buttons to oncrease and decrease the angle by a step
        params.keyBindings.push_back({.uniform = "angle", .keyIncrease = "left", .keyDecrease = "right", .step = 0.01f});
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
