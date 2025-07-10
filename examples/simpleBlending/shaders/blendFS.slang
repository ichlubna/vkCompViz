struct Parameters
{
    float factor;
}
[[binding(0)]]
uniform Parameters parameters;

[shader("compute")]
[numthreads(1,1,1)]
void main(uint3 threadId : SV_DispatchThreadID)
{

}
