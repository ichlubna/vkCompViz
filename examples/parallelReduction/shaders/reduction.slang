#include "inputs.slang"

[shader("compute")]
[numthreads(256,1,1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x == 0 && threadID.y == 0 && threadID.z == 0)
    {
        sharedBuffer[0] = buffer[0]+1.0f;
        buffer[0] = sharedBuffer[0];
    }
}
