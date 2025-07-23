#include "inputs.slang"

[shader("compute")]
[numthreads(10,10,1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
//    float2 uv = float2(threadID.xy) / float2(tex.GetDimensions());
    float4 color = float4(0.0, 0.0, 0.0, 1.0); //tex.Sample(texSampler, uv);
    //color.rgb = 1.0 - color.rgb;
    texw[0][threadID.xy] = color;
}
