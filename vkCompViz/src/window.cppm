export module window;

import std;

export namespace Window
{
    class Window
    {
        public:
        class Parameters
        {
            public:
            std::size_t width;
            std::size_t height;
            std::string title;        
        }; 
        Window(const Parameters &parameters) {};
        virtual void run() = 0;
        virtual ~Window() {};
    };
}
