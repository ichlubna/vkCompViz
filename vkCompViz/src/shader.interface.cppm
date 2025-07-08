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
                std::vector<std::uint32_t> code;
                std::size_t uniformBufferSize{0};
                std::vector<std::string> uniformNames;
        };
        Shader(){};
        [[nodiscard]] virtual Info loadFromFile(std::string path) const = 0;
        [[nodiscard]] virtual Info loadFromString(std::string code) const = 0;
        virtual ~Shader() = default;
};
}
