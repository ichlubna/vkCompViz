export module gpu: interface;
export import loader;
import std;

export namespace Gpu
{
class Gpu
{
    public:
        /**
         * @brief Represents the number of workgroups dispatched in each dimension.
         */
        class WorkGroupCount
        {
            public:
                /** Number of workgroups in X dimension. */
                std::size_t x;

                /** Number of workgroups in Y dimension. */
                std::size_t y;

                /** Number of workgroups in Z dimension. */
                std::size_t z;
        };

        /**
         * @brief Reports detailed benchmark statistics for GPU computation and rendering.
         */
        class BenchmarkReport
        {
            public:
                /**
                 * @brief Number of in-flight frames during benchmarking.
                 */
                std::size_t inFlightFrames {1};

                /**
                 * @brief Timing data for different GPU operations in milliseconds.
                 */
                class Times
                {
                    public:
                        /**
                         * @brief Compute times per compute shader iteration in milliseconds.
                         */
                        std::vector<float> compute;

                        /**
                         * @brief Time taken to draw a frame in milliseconds.
                         */
                        float draw{0};

                        /**
                         * @brief Download times from GPU to CPU in milliseconds.
                         */
                        class Download
                        {
                            public:
                                /**
                                 * @brief Time taken to download texture data.
                                 */
                                float texture{0};

                                /**
                                 * @brief Time taken to download shader storage buffer data.
                                 */
                                float shaderStorage{0};
                        } download;

                        /**
                         * @brief Upload times from CPU to GPU in milliseconds.
                         */
                        class Upload
                        {
                            public:
                                /**
                                 * @brief Time taken to upload texture data.
                                 */
                                float texture{0};

                                /**
                                 * @brief Time taken to upload shader storage buffer data.
                                 */
                                float shaderStorage{0};
                        } upload;
                } times;

                /**
                 * @brief Amount of GPU memory used in bytes.
                 */
                float usedMemory {0};

                /**
                 * @brief Returns total compute time in milliseconds.
                 * @return Compute time.
                 */
                [[nodiscard]] float computeTime() const;

                /**
                 * @brief Returns total memory transfer time in milliseconds.
                 * @return Memory transfer time.
                 */
                [[nodiscard]] float memoryTime() const;

                /**
                 * @brief Returns total time including compute and optionally draw time in milliseconds.
                 * @param includeDraw Whether to include draw time in total.
                 * @return Total elapsed time.
                 */
                [[nodiscard]] float totalTime(bool includeDraw = false) const;

                /**
                 * @brief Returns a string representation of the benchmark report.
                 * @return String describing the benchmark data.
                 */
                [[nodiscard]] std::string toString() const;

                /**
                 * @brief Prints the benchmark report to the console.
                 */
                void print() const;

                /**
                 * @brief Saves the benchmark report to the specified file path.
                 * @param path File path to save the report.
                 */
                void store(std::string path) const;
        };

        Gpu() = default;
        virtual void run() = 0;
        virtual void computeSettings(std::vector<WorkGroupCount> shaderWorkGroupCounts, bool runBenchmark) = 0;
        virtual void resize() = 0;
        virtual void updateUniformBuffer(std::vector<std::uint32_t> buffer) = 0;
        virtual void updateUniform(std::string name, float value) = 0;
        virtual void printUniforms() const = 0;
        virtual void addToUniform(std::string name, float value) = 0;
        [[nodiscard]] virtual std::shared_ptr<Loader::Image> resultTexture() = 0;
        [[nodiscard]] virtual std::vector<float> resultBuffer(std::size_t size = 0) = 0;
        virtual ~Gpu() = default;
        /**
         * @brief Returns the benchmark reports collected during execution.
         * @return Vector of benchmark reports.
         */
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
