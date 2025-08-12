struct Parameters
{
    float angle;
    float verticesStart;
    float normalsStart;
    float mouseX;
    float mouseY;
    float lightOverride;
}
[[vk::binding(0, 0)]]
uniform Parameters parameters;
[[vk::binding(5, 0)]]
RWStructuredBuffer<float> buffer;
struct VertexOutput
{
	float4 position : SV_Position;
    float3 normal : NORMAL;
};
