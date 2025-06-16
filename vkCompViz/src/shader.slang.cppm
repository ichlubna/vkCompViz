module;
#include <slang-com-ptr.h>
export module shader: slang;
export import : interface;
import std;

export namespace Shader
{
class SlangFactory : public Shader
{
    public:
        SlangFactory();
        void addShaderSourcePath(std::string path);
        [[nodiscard]] std::vector<uint32_t> loadFromFile(std::string shaderName) const override;
        [[nodiscard]] std::vector<uint32_t> loadFromString(std::string shaderName) const override;
        [[nodiscard]] std::vector<std::uint32_t> defaultShader(DefaultShaderType type) const override;
        ~SlangFactory();

    private:
        Slang::ComPtr<slang::IGlobalSession> globalSession;
        std::vector<std::string> searchPaths;
        [[nodiscard]] Slang::ComPtr<slang::ISession> createSession() const;
        std::vector<uint32_t> compile(slang::IModule *shaderModule, Slang::ComPtr<slang::ISession> session) const;
};
}
