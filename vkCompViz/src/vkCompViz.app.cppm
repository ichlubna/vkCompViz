export module vkCompViz: app;

import std;

export namespace vkCompViz
{
    class App
    {
        public:
        class ComputeParameters
        {

        };
        class WindowParameters
        {
            public:
            std::size_t width {1024};
            std::size_t height {720};
            std::string title {"No title"};
        };
        void run(ComputeParameters const &computeParameters, WindowParameters const &windowParameters);
        void run(ComputeParameters const &computeParameters);
    };

}
