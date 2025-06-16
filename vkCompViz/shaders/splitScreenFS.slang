struct VertexOutput
{
	float4 position : SV_Position;
};


[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
	float screenX = input.position.x / input.position.w;
	float normalizedX = 0.5 * screenX + 0.5;

	if (normalizedX < 0.5)
		return float4(1, 0, 0, 1);
	else
		return float4(0, 0, 0, 1);
}
