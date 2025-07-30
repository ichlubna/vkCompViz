struct Parameters
{
    float amount;
}
[[vk::binding(0, 0)]]
uniform Parameters parameters;
[[vk::binding(1, 0)]]
Texture2D<float4> outputSampler[2];
[[vk::binding(2, 0)]]
RWTexture2D<float4> outputTexture[2];
[[vk::binding(3, 0)]]
SamplerState inputSampler[1];
[[vk::binding(4, 0)]]
Texture2D<float4> inputTexture[1];
