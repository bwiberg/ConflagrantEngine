#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Asset.hh>

namespace cfl {
namespace assets {
class AssetManager;

typedef std::shared_ptr<Asset> (*AssetLoader)(Path const &path);
} // namespace assets
} // namespace cfl
