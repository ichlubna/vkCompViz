module;

#include <string>

export module imageFfmpeg;
export import loader;

// TODO use this one instead of the include above
//import std;

export namespace Loader
{
    class ImageFfmpeg : public Image
    {
        public:
        ImageFfmpeg(std::string path);
        ~ImageFfmpeg();
        const unsigned char* getData() const;
        private:
        unsigned char* data;
        std::size_t width;
        std::size_t height;
    };
}
