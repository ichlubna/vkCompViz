export module gpu: interface;
export import loader;
import std;

export namespace Gpu
{
class Gpu
{
    public:
        class WorkGroupCount
        {
            public:
                std::size_t x;
                std::size_t y;
                std::size_t z;
        };
        class BenchmarkReport
        {
            public:
                std::size_t inFlightFrames {1};
                class Times
                {
                    public:
                        std::vector<float> compute;
                        float textureUpload {0};
                        float shaderStorageUpload {0};
                } times;
                float usedMemory {0};
                [[nodiscard]] float computeTime() const;
                void store(std::string path) const;
        };
        Gpu() = default;
        virtual void run() = 0;
        virtual void computeSettings(std::vector<WorkGroupCount> shaderWorkGroupCounts, bool runBenchmark) = 0;
        virtual void resize() = 0;
        virtual void updateUniformBuffer(std::vector<std::uint32_t> buffer) = 0;
        virtual void updateUniform(std::string name, float value) = 0;
        virtual void addToUniform(std::string name, float value) = 0;
        [[nodiscard]] virtual std::shared_ptr<Loader::Image> resultTexture() = 0;
        [[nodiscard]] virtual std::vector<float> resultBuffer() = 0;
        virtual ~Gpu() = default;
        [[nodiscard]] const std::vector<BenchmarkReport> benchmarkReports() const
        {
            return benchmarks;
        }

    private:
        virtual void init() = 0;
        std::vector<BenchmarkReport> benchmarks;
};
}
