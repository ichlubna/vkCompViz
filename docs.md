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
The class **Parameters** contains all necessary settings for the computation or visualization, as follows:

## class App::Parameters

**parameters.keyBindings**  
**Type:** std::vector&lt;KeyBinding&gt;  
**Description:** List of uniform key bindings so that uniform values can be modified by pressing various buttons. Used only in mode with window.
**Usage:** This code allows pressing A and S to increase and decrease a value of `myUniform` by step 0.01.
`keyBindings.push_back({.uniform = "myUniform", .keyIncrease = "A", .keyDecrease = "S", .step = 0.01f});`

---

**parameters.priorityUUID**  
**Type:** std::string  
**Description:** UUID of a GPU that should be used. 
**Default:** "" (the best device is automatically selected)

## class App::Parameters::Benchmark

**parameters.benchmark.enable**  
**Type:** bool  
**Description:** Whether benchmark mode is enabled. Used only in mode without a window. 

---

**parameters.benchmark.path**  
**Type:** std::string  
**Description:** Output path for benchmark data.  
**Default:** "./"

## class App::Parameters::Screenshot

**parameters.screenshot.path**  
**Type:** std::string  
**Description:** Path where screenshots are saved.  
**Default:** "./"

---

**parameters.screenshot.extension**  
**Type:** std::string  
**Description:** File extension for screenshots.
**Default:** ".png"

## class App::Parameters::Shaders

**parameters.shaders.compute**  
**Type:** std::vector&lt;std::string&gt;  
**Description:** Paths to compute shader files (expected to be in `shaders` directory at the same directory as the resulting binary).  The order of the shaders matters and decides their execution order.

---

**parameters.shaders.workGroupCounts**  
**Type:** std::vector&lt;Gpu::Gpu::WorkGroupCount&gt;  
**Description:** List of work group counts for compute shaders.  The order should correspond to the shaders vector.

---

**parameters.shaders.vertex**  
**Type:** std::string  
**Description:** Path to the vertex shader file. 
**Default:** "fullScreenVS.slang" (creates a fullscreen triangle)

---

**parameters.shaders.fragment**  
**Type:** std::string  
**Description:** Path to the fragment shader file.  
**Default:** "textureDisplayFS.slang" (renders the first output texture on the screen)

---

**parameters.shaders.vertexCount**  
**Type:** std::size_t  
**Description:** Number of vertices used in rendering.  Use only when custom vertex shader is used.

---

**parameters.shaders.iterations**  
**Type:** std::size_t  
**Description:** Number of the compute shaders iterations to run when no window is used.
**Default:** 1

---

**parameters.shaders.uniforms**  
**Type:** std::vector&lt;std::pair&lt;std::string, float&gt;&gt;  
**Description:** List of shader uniform name-value pairs.  
**Usage:** The following code adds an uniform `myUniform` with a default value 42.0. The uniform needs to be present in all shaders.
`params.shaders.uniforms.push_back({"myUniform", 42.0f});`


## class App::Parameters::Shaders::StorageBuffer

**parameters.shaders.storageBuffer.size**  
**Type:** std::size_t  
**Description:** Size of the shader storage buffer.  

---

**parameters.shaders.storageBuffer.initialData**  
**Type:** std::vector&lt;float&gt;  
**Description:** Initial data to fill the storage buffer.  

## class App::Parameters::Textures

**parameters.textures.input**  
**Type:** std::vector&lt;std::string&gt;  
**Description:** Paths to input texture files.  

---

**parameters.textures.output**  
**Type:** std::vector&lt;Output&gt;  
**Description:** List of output texture configurations.  
**Usage:** The following code adds an output texture with the same resolution and size as the first input texture at index 0:
`params.textures.output.push_back({.sameResolutionAsInputID = 0, .sameFormatAsInputID = 0});`
The following code adds a custom defined output texture with resolution of 320x240 and floating point RGBA format (false means 8-bit RGBA):
`params.textures.output.push_back({.resolution = {320, 240}, .floatFormat = true});`

## class App::Parameters::Window

**parameters.window.enable**  
**Type:** bool  
**Description:** Whether the application window is enabled.  

---

**parameters.window.resolution**  
**Type:** Resolution  
**Description:** Resolution of the application window.  

---

**parameters.window.title**  
**Type:** std::string  
**Description:** Title of the application window.  

---

**parameters.window.fps**  
**Type:** float  
**Description:** Frame rate of the application window.  

---

The class **App** has the following functions:

## class App
**app.run(Parameters const &inputParameters)**  
**Description:** Runs the compute and rendering pipelines.

---

**const Resolution getImageResolution(std::string path)**
**Description:** Returns a resolution of an image.

---

**const Shader::Shader::Info::WorkGroupSize getShaderWorkGroupSize(std::string path)**
**Description:** Returns a work group size defines in a shader file.

---

**const Gpu::Gpu::WorkGroupCount calculateWorkGroupCount(Shader::Shader::Info::WorkGroupSize workGroupSize, Shader::Shader::Info::ThreadCount threadCount)**
**Description:** Calculates how many workgroups are necessary for a given workgroup size and number of threads.

---

**void saveResultImage(std::string path)**
**Description:** Stores the first output texture to a file.

---

**std::vector&lt;float&gt; resultBuffer(std::size_t size = 0)**
**Description:** Returns the shader storage buffer. If `size` is zero then whole buffer is returned.

---

**const std::vector&lt;Gpu::Gpu::BenchmarkReport&gt; benchmarkReports()**
**Description:** Returns the reports about performance.

---

The benchmark reports can be used as:

## class BenchmarkReport

**benchmarkReport.inFlightFrames**  
**Type:** std::size_t  
**Description:** Number of in-flight frames.  

---

**benchmarkReport.usedMemory**  
**Type:** float  
**Description:** Amount of GPU memory used in bytes.  

---

**float computeTime()**  
**Description:** Returns the total compute time in milliseconds.  

---

**float memoryTime()**  
**Description:** Returns the total memory transfer time in milliseconds.  

---

**float totalTime(bool includeDraw = false)**  
**Description:** Returns the total time including compute and optionally draw time.  

---

**std::string toString()**  
**Description:** Returns a string representation of the benchmark report.  

---

**void print()**  
**Description:** Prints the benchmark report to the console.  

---

**void store(std::string path)**  
**Description:** Saves the benchmark report to the specified file path.  


## class BenchmarkReport::Times

**benchmarkReport.times.compute**  
**Type:** std::vector&lt;float&gt;  
**Description:** Compute times per compute shader in milliseconds. 

---

**benchmarkReport.times.draw**  
**Type:** float  
**Description:** Time taken to draw a frame in milliseconds.  

## class BenchmarkReport::Times::Download

**benchmarkReport.times.download.texture**  
**Type:** float  
**Description:** Time taken to download texture data in milliseconds.  

---

**benchmarkReport.times.download.shaderStorage**  
**Type:** float  
**Description:** Time taken to download shader storage buffer in milliseconds.  

## class BenchmarkReport::Times::Upload

**benchmarkReport.times.upload.texture**  
**Type:** float  
**Description:** Time taken to upload texture data in milliseconds. 

---

**benchmarkReport.times.upload.shaderStorage**  
**Type:** float  
**Description:** Time taken to upload shader storage buffer in milliseconds. 

---

