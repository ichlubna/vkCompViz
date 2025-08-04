#include "inputs.slang"

[shader("vertex")]
VertexOutput main(uint vertexID : SV_VertexID)
{
    int ID = vertexID*2;
    int vertexID = int(parameters.verticesStart + buffer[ID]*3);
    int normalID = int(parameters.normalsStart + buffer[ID+1]*3);

    float3 vertex = {buffer[vertexID], buffer[vertexID+1], buffer[vertexID+2]};
    float3 normal = {buffer[normalID], buffer[normalID+1], buffer[normalID+2]};

    float cosA = cos(parameters.angle);
    float sinA = sin(parameters.angle);

    float4x4 rotation = float4x4(
        cosA,  0.0f, -sinA, 0.0f,
        0.0f,  1.0f,  0.0f, 0.0f,
        sinA,  0.0f,  cosA, 0.0f,
        0.0f,  0.0f,  0.0f, 1.0f
    );
	VertexOutput output;
    float4 position = float4(vertex, 1.0);
    position += float4(-0.5, -0.5, -0.5, 0.0);
	position = mul(rotation, position);
    position *= 0.9;
    position.z += 0.5;
    position.w = 1.0;
    output.position = position;
    output.normal = mul((float3x3)rotation, normal);
	return output;
}


