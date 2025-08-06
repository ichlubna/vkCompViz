#include <cstdlib>
import arguments;
import vkCompViz;
import std;

class Timer
{
    private:
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point end;
    public:
        Timer()
        {
            start = std::chrono::steady_clock::now();
        }
        float elapsed()
        {
            end = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000.0f;
        }
};

int main(int argc, char *argv[])
{
    try
    {
        std::string help =  "This program computes am average value of an input array of random floats.\n"
                            "Usage: ./parallelReduction -s sizeOfTheArray\n"
                            "Optional parameters -d defines a string containing GPU UUID to be selected\n";
        Arguments args(argc, argv);
        if(args.printHelpIfPresent(help))
            return EXIT_SUCCESS;

        if(!args["-s"])
            throw std::invalid_argument("Missing parameters");

        // Creating a vector of random floats
        std::vector<float> inputData(args["-s"]);
        std::mt19937 rng(std::time(nullptr));
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::generate(inputData.begin(), inputData.end(), [&]()
        {
            return dist(rng);
        });

        // GPU version
        vkCompViz::App::Parameters params;
        // Using the reduction shader
        params.shaders.compute.push_back("reduction.slang");
        params.shaders.compute.push_back("reduction.slang");
        vkCompViz::App app;
        auto workGroupSize = app.getShaderWorkGroupSize("reduction.slang");
        params.shaders.workGroupCounts.push_back(app.calculateWorkGroupCount(workGroupSize, {inputData.size(), 1, 1}));
        params.shaders.workGroupCounts.push_back({1, 1, 1});
        params.shaders.storageBuffer.size = inputData.size() * sizeof(float);
        params.shaders.storageBuffer.initialData = inputData;
        // This app will not use a window, can be also run on headless machines
        params.window.enable = false;
        // The number of the compute pipeline runs in the headless mode (default is one run)
        params.shaders.iterations = 1;
        // Enables benchmarking - measuring of time and memory usage, it is the same as pressing F2 in windowed mode
        // Report is not saved to file but available as showed below
        params.benchmark.enable = true;
        // In case of multiple GPUs available, the best one is automatically selected
        // The devices are listed in the console and the user can override the selection by defining a concrete UUID
        if(args["-d"])
            params.priorityUUID = static_cast<std::string>(args["-d"]);
        app.run(params);
        // Downloads one float from the shader storage buffer (passing no or 0 parameter stores the whole buffer)
        auto result = app.resultBuffer(sizeof(float));
        // The benchmark reports are also available here
        auto benchmarks = app.benchmarkReports();
        //benchmarks[0]....

        std::cerr << inputData[0] << " " << result[0] << std::endl;
        float gpuSum = 0;

        // CPU version
        Timer timer;
        float cpuSum = std::accumulate(inputData.begin(), inputData.end(), 0.0);
        float cpuTime = timer.elapsed();

        // Report
        constexpr size_t precision = 5;
        std::cout << "GPU" << std::endl;
        std::cout << "Sum: " << std::fixed << std::setprecision(precision) <<  gpuSum << std::endl;
        std::cout << "Time: " << benchmarks[0].totalTime() << " ms" << std::endl;
        std::cout << "Only computation: " << benchmarks[0].computeTime() << " ms" << std::endl;
        std::cout << "Only memory: " << benchmarks[0].memoryTime() << " ms" << std::endl;
        std::cout << std::endl;
        std::cout << "CPU" << std::endl;
        std::cout << "Sum: " << std::fixed << std::setprecision(precision) << cpuSum << std::endl;
        std::cout << "Time: " << cpuTime << " ms" << std::endl;
        std::cout << std::endl;
        std::cout << "Total times: ";
        if(benchmarks[0].totalTime() < cpuTime)
            std::cout << "GPU is " << cpuTime/benchmarks[0].totalTime() << "× faster" << std::endl;
        else
            std::cout << "CPU is " << benchmarks[0].totalTime()/cpuTime << "× faster" << std::endl;
        std::cout << "Computation only:";
        if(benchmarks[0].computeTime() < cpuTime)
            std::cout << " GPU is " << cpuTime/benchmarks[0].computeTime() << "× faster" << std::endl;
        else
            std::cout << " CPU is " << benchmarks[0].computeTime()/cpuTime << "× faster" << std::endl;
        
        // The benchmark data can also be printed 
        //benchmarks[0].print();

    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
