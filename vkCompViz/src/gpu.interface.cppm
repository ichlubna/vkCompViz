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
        virtual void setInFlightFrames(std::size_t count) = 0;
        virtual std::size_t addInputTexture(std::shared_ptr<Loader::Image> image) = 0;
        virtual std::size_t addOutputTexture(Loader::Image::ImageFormat imageFormat) = 0;
        virtual void updateUniformBuffer(std::vector<std::uint32_t> buffer) = 0;
        virtual ~Gpu() = default;

    private:
        virtual void init() = 0;
};
}
