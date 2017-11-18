#pragma once

#include <conflagrant/assets/AssetLoader.hh>

namespace cfl {
namespace assets {
std::shared_ptr<Asset> LoadTexture(Path const &path);
} // namespace assets
} // namespace cfl
