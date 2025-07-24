export module shader: interface;
import std;

export namespace Shader
{
class Shader
{
    public:
        class Info
        {
            public:
                class WorkGroupSize
                {
                    public:
                    std::size_t x{0};
                    std::size_t y{0};
                    std::size_t z{0};
                } workGroupSize;
                std::vector<std::uint32_t> code;
                std::size_t uniformBufferSize{0}; 
                std::vector<std::string> uniformNames;
                using ThreadCount = WorkGroupSize;
        };
        Shader() {};
        [[nodiscard]] virtual Info loadFromFile(std::string path) const = 0;
        [[nodiscard]] virtual Info loadFromString(std::string code) const = 0;
        virtual ~Shader() = default;
};
}
