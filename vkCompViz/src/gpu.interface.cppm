export module gpu: interface;
import common;

export namespace Gpu
{
class Gpu
{
    public:
        Gpu() = default;
        virtual void draw() = 0;
        virtual void compute() = 0;
        virtual void resize() = 0;
        virtual ~Gpu() = default;

    private:
        virtual void init() = 0;
};
}
