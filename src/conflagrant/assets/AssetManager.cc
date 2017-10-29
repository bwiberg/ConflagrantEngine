#include "AssetManager.hh"

namespace cfl {
namespace assets {
static std::unordered_map<string, AssetLoader> assetLoadersByExtension;

std::shared_ptr<const Asset> AssetManager::InternalLoadAsset(std::string const &path_) {
    Path path(path_);

    if (!path.exists())
        return nullptr;

    std::string const extension = path.extension();

    if (!SupportsExtension(extension))
        return nullptr;

    std::shared_ptr<const Asset> asset = nullptr;
    if ((asset = GetPreloadedAsset(path_)))
        return asset;

    asset = assetLoadersByExtension[extension](path);
    return loadedAssetsByPath[path_] = asset;
}

std::shared_ptr<const Asset> AssetManager::GetPreloadedAsset(string const &path) {
    auto asset = loadedAssetsByPath.find(path);
    if (asset == loadedAssetsByPath.end()) {
        return nullptr;
    }
    return asset->second;
}

void
AssetManager::RegisterLoaderForExtensions(AssetLoader loader, std::initializer_list<const string> extensions) {
    for (string const &extension : extensions) {
        assetLoadersByExtension[extension] = loader;
    }
}
} // namespace assets
} // namespace cfl
