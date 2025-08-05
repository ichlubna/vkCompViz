module gpu;
import std;
using namespace Gpu;

float Gpu::Gpu::BenchmarkReport::computeTime() const
{
    return std::accumulate(times.compute.begin(), times.compute.end(), 0.0);
}

float Gpu::Gpu::BenchmarkReport::memoryTime() const
{
    return times.download.texture + times.download.shaderStorage + times.upload.texture + times.upload.shaderStorage;
}

float Gpu::Gpu::BenchmarkReport::totalTime(bool includeDraw) const
{
    if(includeDraw)
        return times.draw + computeTime() + memoryTime();
    else
        return computeTime() + memoryTime();
}

std::string Gpu::Gpu::BenchmarkReport::toString() const
{
    std::ostringstream file;
    file << "Frames in flight: " << inFlightFrames << std::endl;
    file << "Draw time: " << times.draw << " ms" << std::endl;
    file << "Compute time: " << computeTime() << " ms" << std::endl;
    for(std::size_t i = 0; i < times.compute.size(); i++)
        file << "\t" << "Shader " << i << ": " << times.compute[i] << " ms" << std::endl;
    file << "Memory transfer time: " << memoryTime() << " ms" << std::endl;
    file << "\t" << "Upload time: " << times.upload.texture + times.upload.shaderStorage << " ms" << std::endl;
    file << "\t\t" << "Texture: " << times.upload.texture << " ms" << std::endl;
    file << "\t\t" << "Shader storage: " << times.upload.shaderStorage << " ms" << std::endl;
    file << "\t" << "Download time: " << times.download.texture + times.download.shaderStorage << " ms" << std::endl;
    file << "\t\t" << "Texture: " << times.download.texture << " ms" << std::endl;
    file << "\t\t" << "Shader storage: " << times.download.shaderStorage << " ms" << std::endl;
    file << "Total time: " << totalTime() << " ms" << std::endl;
    file << "Used memory" << std::endl;
    file << "\t" << usedMemory << " B" << std::endl;
    file << "\t" << usedMemory/1024 << " kB" << std::endl;
    file << "\t" << usedMemory/(1024*1024) << " MB" << std::endl;
    return file.str();
}

void Gpu::Gpu::BenchmarkReport::print() const
{
    std::cout << toString() << std::endl;
}

void Gpu::Gpu::BenchmarkReport::store(std::string path) const
{
    std::ofstream file;
    file.open(path);
    file << toString();
    file.close();
    std::cout << "Report stored to " << path << std::endl;
}
