import vertexFragmentCommons;
  
[[vk::binding(1, 0)]] 
SamplerState texSampler[];
[[vk::binding(1, 0)]] 
Texture2D<float4> tex[]; 

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    return tex[0].Sample(texSampler[0], input.uv);
}
