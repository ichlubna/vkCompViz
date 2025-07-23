export module gpu: interface;
export import loader;
import std;

export namespace Gpu
{
class Gpu
{
    public:
        class WorkGroupCount
        {
            public:
                std::size_t x;
                std::size_t y;
                std::size_t z;
        };
        Gpu() = default;
        virtual void draw() = 0;
        virtual void compute(std::vector<WorkGroupCount> workGroupCounts) = 0;
        virtual void resize() = 0;
        virtual void setInFlightFrames(std::size_t count) = 0;
        virtual void updateUniformBuffer(std::vector<std::uint32_t> buffer) = 0;
        virtual void updateUniform(std::string name, float value) = 0;
        virtual ~Gpu() = default;

    private:
        virtual void init() = 0;
};
}
