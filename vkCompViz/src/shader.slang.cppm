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
        [[nodiscard]] Shader::Info loadFromFile(std::string shaderName) const override;
        [[nodiscard]] Shader::Info loadFromString(std::string shaderName) const override;
        ~SlangFactory();

    private:
        Slang::ComPtr<slang::IGlobalSession> globalSession;
        std::vector<std::string> searchPaths;
        [[nodiscard]] Slang::ComPtr<slang::ISession> createSession() const;
        Shader::Info compile(slang::IModule *shaderModule, Slang::ComPtr<slang::ISession> session) const;
};
}
