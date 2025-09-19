// Including uniforms and other inputs shared in all shaders
#include "inputs.slang"

// This compute shader performs parallel reduction on the input data
[shader("compute")]

// The workgroup uses 1024 threads
[numthreads(1024,1,1)]
void main(uint3 threadID : SV_DispatchThreadID, uint3 localID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
	// Out of range check
    if (threadID.x >= uint(parameters.count))
        return;
    
	// Loading pairs of values, summing and storing to shared memory
    uint globalID = threadID.x*2;
    if (globalID >= uint(parameters.count))
        sharedBuffer[localID.x] = 0.0f; 
    else if (globalID + 1 >= uint(parameters.count))
        sharedBuffer[localID.x] = buffer[globalID]; 
    else
        sharedBuffer[localID.x] = buffer[globalID] + buffer[globalID + 1];

	// Standard parallel reduction code
    for (uint stride = 512; stride > 0; stride >>= 1)
    {
        GroupMemoryBarrierWithGroupSync();
        if (localID.x < stride)
        {
            sharedBuffer[localID.x] += sharedBuffer[localID.x + stride];
        }
    }

	// Storing the result to the memory
    buffer[uint(parameters.count)+groupID.x] = sharedBuffer[0]; 
}
