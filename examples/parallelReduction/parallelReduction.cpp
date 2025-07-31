#include <cstdlib>
import arguments;
import vkCompViz;
import std;

int main(int argc, char *argv[])
{
    try
    {
        std::string help =  "This program computes am average value of an input array of random floats.\n"
                            "Usage: ./parallelReduction -s sizeOfTheArray\n";
        Arguments args(argc, argv);
        if(args.printHelpIfPresent(help))
            return EXIT_SUCCESS;

        if(!args["-s"])
            throw std::invalid_argument("Missing parameters");

        // Creating a vector of random floats
        std::vector<float> inputData(args["-s"]);
        std::mt19937 rng(std::time(nullptr)); 
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        std::generate(inputData.begin(), inputData.end(), [&]() {return dist(rng);}); 
 
        // GPU version
        vkCompViz::App::Parameters params;
        // Using the reduction shader
        params.shaders.compute.push_back("reduction.slang");
        vkCompViz::App app;
        auto workGroupSize = app.getShaderWorkGroupSize("reduction.slang");
        params.shaders.workGroupCounts = {app.calculateWorkGroupCount(workGroupSize, {inputData.size(), 1, 1})};
        params.shaders.storageBuffer.size = inputData.size() * sizeof(float);
        params.shaders.storageBuffer.initialData = inputData;
        // This app will not use a window, can be also run on headless machines, only one iteration is run
        params.window.enable = false;
        app.run(params);       
        auto result = app.resultBuffer(); 
        auto benchmarks = app.benchmarkReports();
        //benchmarks[0]....

        float gpuAverage = 0;
        float gpuTime = 0;

        // CPU version
        auto start = std::chrono::steady_clock::now();
        float sum = std::accumulate(inputData.begin(), inputData.end(), 0.0);
        float cpuAverage = sum / inputData.size();
        auto end = std::chrono::steady_clock::now();
        float cpuTime = std::chrono::duration<float, std::milli>(end - start).count();  
       
         // Report
        std::cout << "GPU" << std::endl;
        std::cout << "Average: " << gpuAverage << std::endl;
        std::cout << "Time: " << gpuTime << " ms" << std::endl;

        std::cout << "CPU" << std::endl;
        std::cout << "Average: " << cpuAverage << std::endl;
        std::cout << "Time: " << cpuTime << " ms" << std::endl;
        
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
