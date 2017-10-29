#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/geometry/Mesh.hh>
#include <conflagrant/assets/Asset.hh>
#include <conflagrant/assets/AssetLoader.hh>
#include <conflagrant/assets/Image.hh>

#include <unordered_map>
#include <initializer_list>

namespace cfl {
namespace assets {

using geometry::Mesh;
using assets::Image2D;

/**
 * @brief //todo add brief description
 */
class AssetManager {
private:
    std::unordered_map<string, std::shared_ptr<Asset const>> loadedAssetsByPath;

    static std::unordered_map<string, AssetLoader> assetLoadersByExtension;
protected:

    /**
     * Internal version of the #LoadAsset member function.
     * @param path The path to the asset (relative or absolute)
     * @return The loaded asset (nullptr if nonexistant)
     */
    virtual std::shared_ptr<const Asset> InternalLoadAsset(string const& path);

    /**
     * Gets an asset that has already been loaded into CPU memory.
     * @param path The path to the asset (relative or absolute)
     * @return The loaded asset (nullptr if nonexistant)
     */
    std::shared_ptr<const Asset> GetPreloadedAsset(string const& path);


public:
    /**
     * Loads the asset located at the given path.
     * @tparam T The type of asset
     * @param path The path to the asset
     * @return a shared pointer to the asset (nullptr if nonexistant)
     */
    template<typename T>
    std::shared_ptr<const T> LoadAsset(string const& path);

    /**
     * Check if the manager supports the given file extension.
     * @param extension The file extension to check
     * @return true if it supports the extension
     */
    bool SupportsExtension(string const& extension) const;

    /**
     * Register an asset loader function to be used for the given file extensions.
     * @param loader The loader to be used
     * @param extensions The extensions for which to use the given loader
     */
    static void RegisterLoaderForExtensions(AssetLoader loader, std::initializer_list<const string> extensions);
};

template<typename T>
std::shared_ptr<const T> AssetManager::LoadAsset(string const &path) {
    static_assert(std::is_base_of<Asset, T>::value, "Asset type must inherit from the Asset interface");
    return std::dynamic_pointer_cast<T>(InternalLoadAsset(path));
}

bool AssetManager::SupportsExtension(string const &extension) const {
    return assetLoadersByExtension.find(extension) != assetLoadersByExtension.end();
}
} // namespace assets
} // namespace cfl
