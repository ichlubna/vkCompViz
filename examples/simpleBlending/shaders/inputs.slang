// Input uniforms
struct Parameters
{
	// The weight determining how are the two images blended
    float factor;
    // Zoom of the result
    float zoom;
    // The steps affect how fast the user zooming is
    float zoomSteps;
    // Mouse position affects the zoomed image position in the window
    float mouseX;
    float mouseY;
}
// Vulkan binding for uniform buffer
[[vk::binding(0, 0)]]
uniform Parameters parameters;
// Vulkan binding for output texture sampler
[[vk::binding(1, 0)]]
Texture2D<float4> outputSampler[1];
// Vulkan binding for output texture object (writable)
[[vk::binding(2, 0)]]
RWTexture2D<float4> outputTexture[1];
// Vulkan binding for input texture sampler
[[vk::binding(3, 0)]]
SamplerState inputSampler[2];
// Vulkan binding for input texture object
[[vk::binding(4, 0)]]
Texture2D<float4> inputTexture[2];
// Vulkan binding for shader storage buffer, not used in the shader
[[vk::binding(5, 0)]]
RWStructuredBuffer<float> buffer;
