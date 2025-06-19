export module shader: interface;
import std;

export namespace Shader
{
class Shader
{
    public:
        Shader(){};
        [[nodiscard]] virtual std::vector<std::uint32_t> loadFromFile(std::string path) const = 0;
        [[nodiscard]] virtual std::vector<std::uint32_t> loadFromString(std::string code) const = 0;
        virtual ~Shader() = default;
};
}
