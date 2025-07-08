import vertexFragmentCommons;
struct Parameters
{
    float test;
}
[[binding(0)]]
uniform Parameters parameters;

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
	if (input.uv.x < parameters.test)
		return float4(1, 0, 0, 1);
	else
		return float4(0, 0, 0, 1);
}
