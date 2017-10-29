#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Asset.hh>

namespace cfl {
namespace assets {
typedef std::shared_ptr<Asset> (*AssetLoader)(Path const& path);
} // namespace assets
} // namespace cfl
