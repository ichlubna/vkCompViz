#include "inputs.slang"

[shader("compute")]
[numthreads(1024,1,1)]
void main(uint3 threadID : SV_DispatchThreadID, uint3 localID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
    sharedBuffer[localID.x] = 0.0;

    float localSum = 0.0;
    for (uint i = localID.x; i < uint(parameters.reducedCount); i += 1024)
        localSum += buffer[uint(parameters.count)+i];

    sharedBuffer[localID.x] = localSum;
    GroupMemoryBarrierWithGroupSync();

    for (uint stride = 512; stride > 0; stride >>= 1)
    {
        GroupMemoryBarrierWithGroupSync();

        if (localID.x < stride)
            sharedBuffer[localID.x] += sharedBuffer[localID.x + stride];
    }

    if (localID.x == 0)
        buffer[0] = sharedBuffer[0];
}
