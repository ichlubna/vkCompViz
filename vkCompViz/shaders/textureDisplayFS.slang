import vertexFragmentCommons;
   
SamplerState texSampler : register(s1);
Texture2D<float4> tex : register(t1); 

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    return tex.Sample(texSampler, input.uv);
}
