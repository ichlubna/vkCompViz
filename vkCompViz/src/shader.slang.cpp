module;
#include <slang.h>
#include <slang-cpp-types.h>
#include <slang-com-ptr.h>
#include <slang-gfx.h>
module shader;
using namespace Shader;

SlangFactory::SlangFactory()
{
    SlangGlobalSessionDesc desc = {};
    slang::createGlobalSession(&desc, globalSession.writeRef());
    if(!globalSession)
        throw std::runtime_error("Could not create global session in Slang");
    addShaderSourcePath("shaders/");
}

void SlangFactory::addShaderSourcePath(std::string path)
{
    searchPaths.push_back(path);
}

Slang::ComPtr<slang::ISession> SlangFactory::createSession() const
{
    slang::SessionDesc sessionDesc;
    slang::TargetDesc targetDesc;
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = globalSession->findProfile("glsl_450");
    sessionDesc.targets = &targetDesc;
    sessionDesc.targetCount = 1;

    std::vector<const char*> paths;
    for (const auto& path : searchPaths)
        paths.push_back(path.c_str());

    sessionDesc.searchPaths = paths.data();
    sessionDesc.searchPathCount = paths.size();
    Slang::ComPtr<slang::ISession> session;
    globalSession->createSession(sessionDesc, session.writeRef());
    return session;
}

std::vector<uint32_t> SlangFactory::loadFromFile(std::string shaderName) const
{
    auto session = createSession();
    Slang::ComPtr<slang::IBlob> diagnostics;
    slang::IModule *shaderModule = session->loadModule(shaderName.c_str(), diagnostics.writeRef());
    if(!shaderModule)
    {
        std::cerr << reinterpret_cast<const char*>(diagnostics->getBufferPointer()) << std::endl;
        throw std::runtime_error("Could not load shader");
    }
    return compile(shaderModule, session);
}

std::vector<uint32_t> SlangFactory::loadFromString(std::string code) const
{
    auto session = createSession();
    Slang::ComPtr<slang::IBlob> diagnostics;
    slang::IModule *shaderModule = session->loadModuleFromSourceString("", "", code.c_str(), diagnostics.writeRef());
    if(!shaderModule)
    {
        std::cout << std::endl << reinterpret_cast<const char*>(diagnostics->getBufferPointer()) << std::endl;
        throw std::runtime_error("Could not load shader");
    }
    return compile(shaderModule, session);
}

std::vector<uint32_t> SlangFactory::compile(slang::IModule *shaderModule, Slang::ComPtr<slang::ISession> session) const
{
    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    shaderModule->findEntryPointByName("main", entryPoint.writeRef());
    slang::IComponentType* components[] = { shaderModule, entryPoint };
    Slang::ComPtr<slang::IComponentType> program;
    session->createCompositeComponentType(components, 2, program.writeRef());
    Slang::ComPtr<slang::IBlob> spirvCode;
    program->getEntryPointCode(0, 0, spirvCode.writeRef(), nullptr);
    const uint32_t* data = reinterpret_cast<const uint32_t*>(spirvCode->getBufferPointer());
    std::vector<uint32_t> code(data, data+spirvCode->getBufferSize());
    if(code.empty())
        throw std::runtime_error("Failed to compile shader");
    return code;
}

std::vector<std::uint32_t> SlangFactory::defaultShader(DefaultShaderType type) const
{ 
    switch(type)
    {
        case DefaultShaderType::VertexFullScreen:
            return loadFromFile("fullScreenVS.slang");
        case DefaultShaderType::FragmentSplitScreen:
            return loadFromFile("splitScreenFS.slang");
        case DefaultShaderType::FragmentTextureDisplay:
            return loadFromFile("textureDisplayFS.slang");
        default:
            throw std::runtime_error("Requested shader not implemented"); 
    }
}

SlangFactory::~SlangFactory()
{
}
