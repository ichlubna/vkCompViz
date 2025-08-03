struct Parameters
{
    float angle;
}
[[vk::binding(0, 0)]]
uniform Parameters parameters;
[[vk::binding(5, 0)]]
RWStructuredBuffer<float> buffer;
struct VertexOutput
{
	float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};
