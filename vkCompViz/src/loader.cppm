module;
#include<string>

export module loader;

//import std;

export namespace Loader
{
    class Image
    {
        public:
        Image(std::string path) {};
        ~Image() {};
        virtual const unsigned char* getData() const = 0;
    };
}
