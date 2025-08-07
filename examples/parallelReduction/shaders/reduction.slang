#include "inputs.slang"

[shader("compute")]
[numthreads(1024,1,1)]
void main(uint3 threadID : SV_DispatchThreadID, uint3 localID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
    if (threadID.x >= uint(parameters.count))
        return;
    
    uint globalID = threadID.x*2;
    if (globalID >= uint(parameters.count))
        sharedBuffer[localID.x] = 0.0f; 
    else if (globalID + 1 >= uint(parameters.count))
        sharedBuffer[localID.x] = buffer[globalID]; 
    else
        sharedBuffer[localID.x] = buffer[globalID] + buffer[globalID + 1];

    for (uint stride = 512; stride > 0; stride >>= 1)
    {
        GroupMemoryBarrierWithGroupSync();
        if (localID.x < stride)
        {
            sharedBuffer[localID.x] += sharedBuffer[localID.x + stride];
        }
    }

    buffer[uint(parameters.count)+groupID.x] = sharedBuffer[0]; 
}
