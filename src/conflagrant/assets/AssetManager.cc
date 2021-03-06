#include "AssetManager.hh"
#include <conflagrant/assets/loaders/ModelLoader.hh>
#include <conflagrant/assets/loaders/TextureLoader.hh>

namespace cfl {
namespace assets {
std::unordered_map<string, AssetLoader> AssetManager::AssetLoadersByExtension;

std::unordered_map<string, std::shared_ptr<Asset>> AssetManager::LoadedAssetsByPath;

PathResolver AssetManager::AssetsPathResolver;

std::shared_ptr<Asset> AssetManager::InternalLoadAsset(std::string const &path_) {
    $
    Path assetPath(path_);
    assetPath = AssetsPathResolver.resolve(assetPath);

    if (!assetPath.exists())
        return nullptr;

    std::string const extension = assetPath.extension();

    if (!SupportsExtension(extension))
        return nullptr;

    auto asset = GetPreloadedAsset(path_);
    if (asset != nullptr) {
        return asset;
    }

    asset = AssetLoadersByExtension[extension](assetPath);
    auto &a = *asset;
    LoadedAssetsByPath[path_] = asset;
    return asset;
}

std::shared_ptr<Asset> AssetManager::GetPreloadedAsset(string const &path) {
    $
    auto asset = LoadedAssetsByPath.find(path);
    if (asset == LoadedAssetsByPath.end()) {
        return nullptr;
    }
    return asset->second;
}

void
AssetManager::RegisterLoaderForExtensions(AssetLoader loader, std::initializer_list<const string> extensions) {
    $
    for (string const &extension : extensions) {
        AssetLoadersByExtension[extension] = loader;
    }
}

bool AssetManager::AddAssetsPath(Path const &path) {
    $
    assert(path.exists() && path.is_directory());
    AssetsPathResolver.append(path);
    return true;
}

bool AssetManager::SupportsExtension(string const &extension) {
    $
    return AssetLoadersByExtension.find(extension) != AssetLoadersByExtension.end();
}
} // namespace assets
} // namespace cfl
