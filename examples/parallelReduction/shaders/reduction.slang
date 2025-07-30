#include "inputs.slang"

[shader("compute")]
[numthreads(16,16,1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    uint outputTextureWidth, outputTextureHeight;
    outputTexture[0].GetDimensions(outputTextureWidth, outputTextureHeight);
    if (threadID.x >= outputTextureWidth || threadID.y >= outputTextureHeight)
        return; 

    uint textureWidth, textureHeight;
    inputTexture[0].GetDimensions(textureWidth, textureHeight);
    float2 uv = float2(threadID.xy) / float2(textureWidth, textureHeight);
    float4 colorA = inputTexture[0].Sample(inputSampler[0], uv);
    float4 colorB = inputTexture[1].Sample(inputSampler[1], uv);
    float4 color = lerp(colorA, colorB, parameters.factor);
    outputTexture[0][threadID.xy] = color;
}
