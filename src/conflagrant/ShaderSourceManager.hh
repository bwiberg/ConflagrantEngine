#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/gl/Shader.hh>

#include <unordered_map>
#include <list>
#include <unordered_set>

namespace cfl {
class ShaderSourceManager {
    ShaderSourceManager() = delete;

    static PathResolver Resolver;

public:
    static bool RegisterShaderFolder(string const& pathToFolder);

    static bool PrecompileShader(string const &pathToShaderSource, string &out);
};

std::shared_ptr<gl::Shader> LoadShader(string const &vertexPathStr,
                                       string const &fragmentPathStr);

std::shared_ptr<gl::Shader> LoadShader(string const &vertexPathStr,
                                       string const &geometryPathStr,
                                       string const &fragmentPathStr);

std::shared_ptr<gl::Shader> LoadComputeShader(string const &computePathStr);

#define REGISTER_SHADER_FOLDER(path) cfl::ShaderSourceManager::RegisterShaderFolder(path)
} // namespace cfl
