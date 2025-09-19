# Temporary notes
The current version of the project needs to use CMake 4.1.1. This will be fixed in the future when CMake support for import of std module is no longer an experimental feature. See the CMakeLists.txt file for more details. Compiling the project in Release mode might also be problematic due to the use of currently unfinished std module import.

# Vulkan GPGPU Compute Framework
Vulkan Compute Vizualization (vkCompViz) is a Vulkan library for GPU-accelerated data processing. 
The main purpose are computer-graphics-related GPU-accelerated operations on images or other data. 
This framework can be used for a fast experimental prototyping of scientific research ideas. 
Using GPU frameworks requires a lot of boilerplate code and API calls. 
This library simplifies the process so that the user only specifies the input and output data, writes the shader code and runs the pipeline. 
The presented examples can also be edited to quickly create an GPU-based experiment without a time-consuming GPU API documentation study. 

The library can load input images, run a sequence of compute shaders, display the result in window, and store the result. 
Uniform parameters can be interactively changed in console with the result updated in the display window. 
The application can run in headless mode or with window. 
The library can also generate memory and time performance reports.

## How to run
Simple compilation of examples:
```
git clone --recursive https://github.com/ichlubna/vkCompViz.git
cd vkCompViz
mkdir build
cmake .. -G "Ninja"
ninja
cd ./examples/exampleName/
./exampleName
```
More detailed instructions can be found [here](/INSTALL.md).

A path to Artistic Style (astyle) program can be defined with `ccmake` and the project can be formatted using:
```
ninja format
```
## Usage
See the existing examples for reference. Especially, [Simple Blending](examples/simpleBlending), [Parallel Reduction](examples/parallelReduction), and [3D Viewer](examples/3DViewer) are simple and contain explanatory comments. It is recommended to use them as templates for new examples. The API of the library is described in the [documentation](https://ichlubna.github.io/vkCompViz/classvkCompViz_1_1App.html).

A simple basic code using this library, with no window output, that loads an image, processes it with a compute shader and one workgroup, and stores the result is:

    import vkCompViz;
    void main()
    {
		vkCompViz::App::Parameters params;
		params.shaders.compute.push_back("shader.slang");
		params.shaders.workGroupCounts.push_back({1, 1, 1});
		params.textures.input.push_back("input.jpg");
		params.textures.output.push_back({.sameResolutionAsInputID = 0, .sameFormatAsInputID = 0});
		vkCompViz::App app;
		app.run(params);
		app.saveResultImage("output.jpg");
    }

The bindings in shaders are:  

| Binding | Resource |
|---|---|
| 0 | Uniform buffer (One common buffer for all shaders) | 
| 1 | Sampler for output textures | 
| 2 | Writeable output textures | 
| 3 | Sampler for input textures | 
| 4 | Readable input textures | 
| 5 | Shader storage buffer |

The first output texture is considered the final one and is displayed in the window. Its content is also stored to file.  

Keys for window application:  

| Key | Action |
|---|---|
| Esc | Exit program | 
| Space | Edit uniforms in runtime | 
| F1 | Store the the result |
| F2 | Store benchmark report | 
| F3 | Print uniforms | 

## Community guidelines
This repository is primarily for research purposes. 
If you’d like to collaborate or discuss, please open an issue or contact me at [ichlubna@fit.vut.cz](mailto:ichlubna@fit.vut.cz). 
Pull requests are welcome. 
To keep the same formatting style, please use `ninja format` command before contributing. 
All pull requests must pass the automated checks in this repository. 
When using this library in a project, please [cite](/CITATION.cff) this repository.

## Possible future features
- RTX pipeline that can render to texture.
- Add a class reflecting the input buffers and textures which can be asynchronously updated in runtime. GPU would update its data when changes happen. This can allow for data changes during runtime (e.g. video player).
