// Input uniforms
struct Parameters
{
	// Geometry rotation angle
    float angle;
    // Index into the buffer where vertex data start
    float verticesStart;
    // Index into the buffer where normal data start
    float normalsStart;
    // Mouse position when clicked
    float mouseX;
    float mouseY;
    // The mouse was clicked - override light position trigger
    float lightOverride;
}
// Vulkan binding for uniform buffer
[[vk::binding(0, 0)]]
uniform Parameters parameters;
// Vulkan binding for general shader storage buffer
[[vk::binding(5, 0)]]
RWStructuredBuffer<float> buffer;
// Vertex shader output structure
struct VertexOutput
{
	float4 position : SV_Position;
    float3 normal : NORMAL;
};
