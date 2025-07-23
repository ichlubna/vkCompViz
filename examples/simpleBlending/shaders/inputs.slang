struct Parameters
{
    float factor;
}
[[vk::binding(0, 0)]]
uniform Parameters parameters;
[[vk::binding(1, 0)]]
SamplerState texSampler[1];
[[vk::binding(1, 0)]]
Texture2D<float4> tex[1];
[[vk::binding(2, 0)]]
RWTexture2D<float4> texw[1];
