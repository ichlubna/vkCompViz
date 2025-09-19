// Including uniforms and other inputs shared in all shaders
#include "inputs.slang"

// This compute shader performs second-stage parallel reduction on the intermediate results
[shader("compute")]

// The workgroup uses 1024 threads, only one workgroup is invoked
[numthreads(1024,1,1)]
void main(uint3 threadID : SV_DispatchThreadID, uint3 localID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
	// Cleaning shared memory
    sharedBuffer[localID.x] = 0.0;

	// Summing manually the remaining values after the first stage reduction
    float localSum = 0.0;
    for (uint i = localID.x; i < uint(parameters.reducedCount); i += 1024)
        localSum += buffer[uint(parameters.count)+i];

    sharedBuffer[localID.x] = localSum;
    GroupMemoryBarrierWithGroupSync();

	// Running standard parallel reduction on the calculated sums
    for (uint stride = 512; stride > 0; stride >>= 1)
    {
        GroupMemoryBarrierWithGroupSync();

        if (localID.x < stride)
            sharedBuffer[localID.x] += sharedBuffer[localID.x + stride];
    }

	// Using just one thread to store the results
    if (localID.x == 0)
        buffer[0] = sharedBuffer[0];
}
