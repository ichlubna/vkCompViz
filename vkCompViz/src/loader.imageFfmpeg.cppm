module;
#include <string>
#include <memory>
#include "ffmpeg.h"
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
        ImageFfmpeg(size_t width, size_t height, [[maybe_unused]] size_t stride, Format imageFormat, std::string path, uint8_t *data = nullptr);
        [[nodiscard]] const unsigned char *data() const
        {
            return frame->data[0];
        }
        [[nodiscard]] virtual size_t width() const
        {
            return frame->width;
        }
        [[nodiscard]] virtual size_t height() const
        {
            return frame->height;
        }
        [[nodiscard]] virtual size_t stride() const
        {
            return frame->linesize[0];
        }
        [[nodiscard]] virtual Format imageFormat() const
        {
            return format;
        };
        void save(std::string path = "") const override;
        ~ImageFfmpeg();

    private:
        AVFrame *frame;
        Image::Format format;
        std::string path;
};
}
