#include "ShaderSourceManager.hh"

#include <fstream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <deque>

namespace cfl {
namespace Tokens {
static string const Include = "#include ";
}

PathResolver ShaderSourceManager::Resolver;

bool ShaderSourceManager::RegisterShaderFolder(string const &pathToFolder) {
    Path path(pathToFolder);

    if (!path.exists()) {
        LOG_INFO(cfl::ShaderSourceManager::RegisterShaderFolder) << "Path does not exist";
        return false;
    }

    if (!path.is_directory()) {
        LOG_INFO(cfl::ShaderSourceManager::RegisterShaderFolder) << "Path is not a directory";
        return false;
    }

    Resolver.append(path);
    return true;
}

struct ShaderInfo {
    std::vector<string> lines;
    std::map<size_t, string> includePathsByLine;

    static bool CreateFromSource(string const& source, ShaderInfo &out) {
        std::vector<string> lines;
        std::map<size_t, string> includePathsByLine;

        std::stringstream shaderIn(source);

        size_t lineCount = 0;
        string line;

        // read source file, scan for includes
        while (std::getline(shaderIn, line)) {
            auto lineNumber = lineCount++;

            // option 1: line doesn't have include statement, just save it
            if (line.find(Tokens::Include) != 0) {
                lines.emplace_back(line);
                continue;
            }

            // option 2: line has include statement

            // - push empty line to ensure "lines" corresponds with includePathsByLine
            lines.emplace_back();

            // - extract <path> from '#include "<path>"'
            auto lineIncludePart = line.substr(Tokens::Include.size());
            if (lineIncludePart.find('"') != 0 ||
                lineIncludePart.find('"', 1) != lineIncludePart.size() - 1) {
                LOG_ERROR(cfl::ShaderSourceManager::Preprocess) << "Malformed #include statement '" << line << "'";
                return false;
            }

            // - store <path> by the line number
            string toIncludePath = lineIncludePart.substr(1, lineIncludePart.size() - 2);
            includePathsByLine[lineNumber] = toIncludePath;
        }

        out.lines = std::move(lines);
        out.includePathsByLine = std::move(includePathsByLine);
        return true;
    }

    void PushIncludePaths(std::deque<std::pair<size_t, string>> &deque) const {
        for (auto it = includePathsByLine.rbegin(); it != includePathsByLine.rend(); ++it) {
            deque.push_front(*it);
        }
    }
};

bool GetShaderSource(PathResolver const& resolver, string const& pathToShaderSource, string &out) {
    Path path(pathToShaderSource);
    path = resolver.resolve(path);

    if (!path.exists()) {
        LOG_INFO(cfl::ShaderSourceManager::RegisterShaderSource) << "Path does not exist (" << path << ")";
        return false;
    }

    if (!path.is_file()) {
        LOG_INFO(cfl::ShaderSourceManager::RegisterShaderSource) << "Path is not a file (" << path << ")";
        return false;
    }

    std::ifstream file(path.str());
    if (!file.is_open()) {
        LOG_INFO(cfl::ShaderSourceManager::RegisterShaderSource) << "Failed to open path for reading (" << path << ")";
        return false;
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    out = ss.str();
    return true;
}

bool Preprocess(PathResolver const& resolver,
                ShaderInfo const &info, std::stringstream &out,
                std::unordered_map<string, ShaderInfo> const &shaderInfosByPath,
                std::unordered_set<string> &included) {
    auto N = info.lines.size();
    for (decltype(N) i = 0; i < N; i++) {
        auto it = info.includePathsByLine.find(i);

        if (it == info.includePathsByLine.end()) {
            out << info.lines[i] << std::endl;
            continue;
        }

        auto includePath = it->second;
        if (included.find(includePath) != included.end()) {
            // already included
            continue;
        }

        string source;
        if (!GetShaderSource(resolver, includePath, source)) {
            return false;
        }

        ShaderInfo info;
        if (!ShaderInfo::CreateFromSource(source, info)) {
            return false;
        }

        included.insert(includePath);
        if (!Preprocess(resolver, info, out, shaderInfosByPath, included)) {
            return false;
        }
    }

    return true;
}

bool ShaderSourceManager::PrecompileShader(string const &pathToShaderSource, string &out) {
    string source;
    if (!GetShaderSource(Resolver, pathToShaderSource, source)) {
        return false;
    }

    ShaderInfo info;
    if (!ShaderInfo::CreateFromSource(source, info)) {
        return false;
    }

    std::stringstream preprocessed;
    std::unordered_set<string> included;
    std::unordered_map<string, ShaderInfo> shaderInfosByPath;

    shaderInfosByPath[pathToShaderSource] = info;
    included.insert(pathToShaderSource);

    if (!cfl::Preprocess(Resolver, info, preprocessed, shaderInfosByPath, included)) {
        return false;
    }

    out = preprocessed.str();
    return true;
}

std::shared_ptr<gl::Shader> LoadShader(string const &vertexPathStr,
                                       string const &fragmentPathStr) {
    string vertex;
    string fragment;

    if (!ShaderSourceManager::PrecompileShader(vertexPathStr, vertex) ||
        !ShaderSourceManager::PrecompileShader(fragmentPathStr, fragment)) {
        return nullptr;
    }

    return std::make_shared<gl::Shader>(vertex, fragment);
}

std::shared_ptr<gl::Shader> LoadShader(string const &vertexPathStr,
                                       string const &geometryPathStr,
                                       string const &fragmentPathStr) {
    string vertex;
    string fragment;
    string geometry;

    if (!ShaderSourceManager::PrecompileShader(vertexPathStr, vertex) ||
        !ShaderSourceManager::PrecompileShader(geometryPathStr, geometry) ||
        !ShaderSourceManager::PrecompileShader(fragmentPathStr, fragment)) {
        return nullptr;
    }

    return std::make_shared<gl::Shader>(vertex, fragment, geometry);
}

} // namespace cfl
