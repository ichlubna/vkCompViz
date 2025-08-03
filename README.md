# Vulkan GPGPU Compute Framework
Vulkan Compute Vizualization (vkCompViz) is a Vulkan library for GPU-accelerated data processing. The main purpose are computer-graphics-related GPU-accelerated operations on images.

The library can load input images, run a sequence of compute shaders, display the result in window, and store the result. Uniform parameters can be interactively changed in console with the result updated in the display window.

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
The examples can be disabled and enabled using  `ccmake`. New examples can be added similarly to the existing ones. When created new example in the examples directory, the example can be added to the main `CMakeLists.txt` as the rest.

A path to Artistic Style (astyle) program can be defined with `ccmake` and the project can be formatted using:
```
ninja format
```
## Usage
See the existing examples for reference. Especially, [Simple Blending](examples/simpleBlending), [Parallel Reduction](examples/parallelReduction), and [3D Viewer](examples/3DViewer) are simple and contain explanatory comments. It is recommended to use them as templates for new examples.

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

### TODO
- RTX pipeline that can render to texture.
- Add a class reflecting the input buffers and textures which can be asynchronously updated in runtime. GPU would update its data when changes happen. This can allow for data changes during runtime (e.g. video player).
