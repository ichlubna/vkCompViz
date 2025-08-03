module;
#include <string>
export module loader: interface;
//import std;

export namespace Loader
{
class Image
{
    public:
        enum class Format { RGBA_8_INT, RGBA_32_FLOAT };
        Image() {};
        [[nodiscard]] virtual const unsigned char *data() const = 0;
        [[nodiscard]] virtual size_t width() const = 0;
        [[nodiscard]] virtual size_t height() const = 0;
        [[nodiscard]] virtual size_t stride() const = 0;
        [[nodiscard]] virtual size_t channels() const = 0;
        [[nodiscard]] virtual size_t channelSize() const = 0;
        [[nodiscard]] size_t size()
        {
            return height() * (stride() != 0 ? stride() : width()) * channels() * channelSize();
        };
        [[nodiscard]] virtual Format imageFormat() const = 0;
        virtual void save(std::string path) const = 0;
        virtual ~Image() = default;
};
}
