// Input uniforms
struct Parameters
{
	// Size of the input array
    float count;
    // Size of the array for the second stage of the reduction
    float reducedCount;
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
SamplerState inputSampler[1];
// Vulkan binding for input texture object
[[vk::binding(4, 0)]]
Texture2D<float4> inputTexture[1];
// Vulkan binding for general shader storage buffer containing the input array
[[vk::binding(5, 0)]]
RWStructuredBuffer<float> buffer;
// Shared memory for faster storing and reading of intermediate results
groupshared float sharedBuffer[1024];
