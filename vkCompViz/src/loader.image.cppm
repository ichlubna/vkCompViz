module;

#include <string>

export module loader: image;

// TODO use this one instead of the include above
//import std;

export class Image
{
    public:
    Image(std::string path);
    ~Image();
    const unsigned char* getData() const;
    private:
    unsigned char* data;
    std::size_t width;
    std::size_t height;
};
