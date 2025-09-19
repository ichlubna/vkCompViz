// Including uniforms and other inputs shared in all shaders
#include "inputs.slang"

// This fragment shader affects the color and lighting of the rendered geometry
[shader("fragment")]

// This is a standard Phong lighting model
float4 main(VertexOutput input) : SV_Target
{
	// A normal vector of the geometry is normalized, just in case
    float3 N = normalize(input.normal);
    // The position of the light
    float3 L = {-0.4f, -0.4f, -1.0f};
    // If the user clicks somewhere in the scene, the light position is edited according to the mouse position
    if(parameters.lightOverride > 0.01f)
    {
        L.x = 2*parameters.mouseX-1;
        L.y = 2*parameters.mouseY-1;
    }
    // View direction
    float3 V = {0.0f, 0.0f, -1.0f};
    // Reflection of the light from the surface
    float3 R = reflect(-L, N);
    // Diffuse part based on the surface orientation to the light source
    float diffuseStrength = max(dot(N, L), 0.0);
    // Specular part based on the reflection of the light to the camera
    float specularStrength = pow(max(dot(R, V), 0.0), 32.0); 
    // Ambient part describes the default light value without any relation to camera or light sources
    float3 ambient = {0.1, 0.1, 0.1};
    // Calculating diffuse and specular colors
    float3 diffuse = diffuseStrength * float3(1.0, 1.0, 1.0);
    float3 specular = specularStrength * float3(1.0, 1.0, 1.0);
    // The final lighting value for the given fragment
    return float4(ambient + diffuse + specular, 1.0f);
}

