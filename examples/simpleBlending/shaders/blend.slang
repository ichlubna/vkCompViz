#include "inputs.slang"

// This function zooms the coordinates baed on the user defined zoom value and mouse position
float2 zoomCoords(float2 imageCoords, float2 mouseCoords, float zoom)
{
    float zoomFactor = 1.0f/parameters.zoomSteps;
    float normalizedZoom = 1.0f-clamp(zoom*zoomFactor, 0.0f, 1.0f);
    float2 result = imageCoords - mouseCoords;
    result *= normalizedZoom;
    result += mouseCoords;
    return result;
}

[shader("compute")]

// This is a compute shader with a 16x16 workgroup size
[numthreads(16,16,1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	// Out of range check
    uint outputTextureWidth, outputTextureHeight;
    outputTexture[0].GetDimensions(outputTextureWidth, outputTextureHeight);
    if (threadID.x >= outputTextureWidth || threadID.y >= outputTextureHeight)
        return; 
	
	// Getting tetxure sampling coordinates
    uint textureWidth, textureHeight;
    inputTexture[0].GetDimensions(textureWidth, textureHeight);
    float2 uv = float2(threadID.xy) / float2(textureWidth, textureHeight);
    
    // Zooming them
    uv = zoomCoords(uv, float2(parameters.mouseX, parameters.mouseY), parameters.zoom);
    
    // Mixing two input images together
    float4 colorA = inputTexture[0].Sample(inputSampler[0], uv);
    float4 colorB = inputTexture[1].Sample(inputSampler[1], uv);
    float4 color = lerp(colorA, colorB, parameters.factor);
    
	// Storing of the resulting fragment
    outputTexture[0][threadID.xy] = color;
}
