struct Parameters
{
    float factor;
    float zoom;
    float zoomSteps;
    float mouseX;
    float mouseY;
}
[[vk::binding(0, 0)]]
uniform Parameters parameters;
[[vk::binding(1, 0)]]
Texture2D<float4> outputSampler[1];
[[vk::binding(2, 0)]]
RWTexture2D<float4> outputTexture[1];
[[vk::binding(3, 0)]]
SamplerState inputSampler[2];
[[vk::binding(4, 0)]]
Texture2D<float4> inputTexture[2];
[[vk::binding(5, 0)]]
RWStructuredBuffer<float> buffer;
