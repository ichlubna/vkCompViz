#include "inputs.slang"

[shader("fragment")]
float4 main(VertexOutput input) : SV_Target
{
    float3 N = normalize(input.normal);
    float3 L = {-0.4f, -0.4f, -1.0f};
    float3 V = {0.0f, 0.0f, -1.0f};
    float3 R = reflect(-L, N);
    float diffuseStrength = max(dot(N, L), 0.0);
    float specularStrength = pow(max(dot(R, V), 0.0), 32.0); 
    float3 ambient = {0.1, 0.1, 0.1};
    float3 diffuse = diffuseStrength * float3(1.0, 1.0, 1.0);
    float3 specular = specularStrength * float3(1.0, 1.0, 1.0);
    return float4(ambient + diffuse + specular, 1.0f);
}

