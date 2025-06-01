module;
#include<string>

export module loader:interface;

//import std;

export namespace Loader
{
    class Image
    {
        public:
        Image(std::string path) {};
        virtual ~Image() = default;
        virtual const unsigned char* getData() const = 0;
    };
}
