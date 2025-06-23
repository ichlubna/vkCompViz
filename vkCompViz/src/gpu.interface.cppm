export module gpu: interface;
export import loader;
import std;

export namespace Gpu
{
class Gpu
{
    public:
        Gpu() = default;
        virtual void draw() = 0;
        virtual void compute() = 0;
        virtual void resize() = 0;
        virtual std::size_t addInputTexture(std::shared_ptr<Loader::Image> image) = 0;
        virtual std::size_t addOutputTexture(Loader::Image::ImageFormat imageFormat) = 0;
        virtual ~Gpu() = default;

    private:
        virtual void init() = 0;
};
}
