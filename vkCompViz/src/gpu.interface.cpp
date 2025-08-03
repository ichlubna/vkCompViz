module gpu;
import std;
using namespace Gpu;

float Gpu::Gpu::BenchmarkReport::computeTime() const
{
    return std::accumulate(times.compute.begin(), times.compute.end(), 0.0);
}

void Gpu::Gpu::BenchmarkReport::store(std::string path) const
{
    std::ofstream file;
    file.open(path);
    file << "Frames in flight: " << inFlightFrames << std::endl;
    file << "Compute time: " << computeTime() << " ms" << std::endl;
    for(std::size_t i = 0; i < times.compute.size(); i++)
        file << "\t" << "Shader " << i << ": " << times.compute[i] << " ms" << std::endl;
    file << "Upload time: " << times.textureUpload + times.shaderStorageUpload << " ms" << std::endl;
    file << "\t" << "Texture upload time: " << times.textureUpload << " ms" << std::endl;
    file << "\t" << "Shader storage upload time: " << times.shaderStorageUpload << " ms" << std::endl;
    file << "Used memory: " << usedMemory << " MB" << std::endl;
    file.close();
}
