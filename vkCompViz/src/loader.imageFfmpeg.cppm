module;
#include <string>
#include <memory>
#include <glm/glm.hpp>

export module loader: imageFfmpeg;
export import : interface;

// TODO use this one instead of the include above
//import std;

export namespace Loader
{
class ImageData
{
    public:
        ImageData();
        [[nodiscard]] const unsigned char *getData() const;
        ~ImageData();

    private:
        unsigned char *data;

};

class ImageFfmpeg : public Image
{
    public:
        ImageFfmpeg(std::string path);
        ~ImageFfmpeg();
        [[nodiscard]] const unsigned char *getData() const;

    private:
        std::unique_ptr<ImageData> data;
        glm::uvec2 resolution;
};
}
