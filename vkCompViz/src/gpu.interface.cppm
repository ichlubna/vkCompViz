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
                        float draw{0};
                        class Download
                        {
                            public:
                            float texture{0};
                            float shaderStorage{0};
                        } download;
                        class Upload
                        {
                            public:
                            float texture{0};
                            float shaderStorage{0};
                        } upload;
                } times;
                float usedMemory {0};
                [[nodiscard]] float computeTime() const;
                [[nodiscard]] float memoryTime() const;
                [[nodiscard]] float totalTime(bool includeDraw = false) const;
                [[nodiscard]] std::string toString() const;
                void print() const;
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
        [[nodiscard]] virtual std::vector<float> resultBuffer(std::size_t size=0) = 0;
        virtual ~Gpu() = default;
        [[nodiscard]] const std::vector<BenchmarkReport> benchmarkReports() const
        {
            return benchmarks;
        }

    private:
        virtual void init() = 0;
    protected:
        bool benchmark = false;
        std::vector<BenchmarkReport> benchmarks;
};
}
