#include "inputs.slang"

[shader("compute")]
[numthreads(256,1,1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    sharedBuffer[0] = 5;
    buffer[0] = sharedBuffer[0];
}
