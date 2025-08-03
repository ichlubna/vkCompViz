#include "inputs.slang"

[shader("vertex")]
VertexOutput main(uint vertexID : SV_VertexID)
{
	const float2 positions[3] = {{-1.0, -1.0}, {3.0, -1.0}, {-1.0,  3.0}};
    const float2 uvs[3] = {{0.0, 0.0}, {2.0, 0.0}, {0.0, 2.0}};

	VertexOutput output;
	output.position = float4(positions[vertexID], 0.0, 1.0);
    output.uv = uvs[vertexID];
	return output;
}

