// Including uniforms and other inputs shared in all shaders
#include "inputs.slang"

// This is a compute shader with a 16x16 workgroup size
[shader("compute")]
[numthreads(16,16,1)]

// This shader is empty, added only as a placeholder but the 3D geometry can be generated here or edited
// For example, a procedural environment can be generated here and rendered in the viewer
void main(uint3 threadID : SV_DispatchThreadID)
{

}
