module;
#include <string>
#include <memory>
export module loader: imageFfmpeg;
export import : interface;

// TODO use this one instead of the include above
//import std;

export namespace Loader
{
class ImageFfmpeg : public Image
{
    public:
        ImageFfmpeg(std::string path);
        [[nodiscard]] const unsigned char *getData() const override;
        [[nodiscard]] virtual size_t width() const { return dataWidth; }
        [[nodiscard]] virtual size_t height() const { return dataHeight; }
        [[nodiscard]] virtual ImageFormat getImageFormat() const { return imageFormat; };
        ~ImageFfmpeg();

    private:
        unsigned char *data;
        Image::ImageFormat imageFormat;
        size_t dataWidth;
        size_t dataHeight;
};
}
