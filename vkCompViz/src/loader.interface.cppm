module;
#include <string>
export module loader: interface;
//import std;

export namespace Loader
{
class Image
{
    public:
        enum class ImageFormat { RGBA_8_INT, RGBA_32_FLOAT };
        Image(std::string path) {};
        [[nodiscard]] virtual const unsigned char *getData() const = 0;
        [[nodiscard]] virtual size_t width() const = 0;
        [[nodiscard]] virtual size_t height() const = 0;
        [[nodiscard]] virtual ImageFormat getImageFormat() const = 0;
        virtual ~Image() = default;  
};
}
