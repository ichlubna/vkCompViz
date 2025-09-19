// Including uniforms and other inputs shared in all shaders
#include "inputs.slang"

//This vertex shader transforms and projects the geometry
[shader("vertex")]
VertexOutput main(uint vertexID : SV_VertexID)
{
	// Each vertex is represented by two 3D vectors: vertex position and normal direction
    int ID = vertexID*2;
    // To avoid duplicated values, the vectors are indirectly indexed (similarly to optimized OBJ format)
    int vertexID = int(parameters.verticesStart + buffer[ID]*3);
    int normalID = int(parameters.normalsStart + buffer[ID+1]*3);

	// The vectors are read from the buffer
    float3 vertex = {buffer[vertexID], buffer[vertexID+1], buffer[vertexID+2]};
    float3 normal = {buffer[normalID], buffer[normalID+1], buffer[normalID+2]};

	// Standard rotation matrix is computed based on the input angle
    float cosA = cos(parameters.angle);
    float sinA = sin(parameters.angle);

    float4x4 rotation = float4x4(
        cosA,  0.0f, -sinA, 0.0f,
        0.0f,  1.0f,  0.0f, 0.0f,
        sinA,  0.0f,  cosA, 0.0f,
        0.0f,  0.0f,  0.0f, 1.0f
    );
    
    // The output vertex
	VertexOutput output;
	
	// The geometry is shifted to the center of NDC
    float4 position = float4(vertex, 1.0);
    position += float4(-0.5, -0.5, -0.5, 0.0);
    
    // The geometry is rotated
	position = mul(rotation, position);
	
	// The geometry is scaled down to fully fit the window even after rotation
    position *= 0.9;
    
    // The geometry is shifted in front of the camera
    position.z += 0.5;
    
    // Using homogeneous coordinates
    position.w = 1.0;
    output.position = position;
    
    // The normal is also rotated
    output.normal = mul((float3x3)rotation, normal);
	return output;
}


