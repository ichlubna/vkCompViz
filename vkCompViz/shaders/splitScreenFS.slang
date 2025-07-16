import vertexFragmentCommons;

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    if (input.uv.x < 0.5f)
		return float4(1, 0, 0, 1);
	else
		return float4(0, 0, 0, 1);
}
