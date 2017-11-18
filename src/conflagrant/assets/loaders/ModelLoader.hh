//
// Created by bwiberg on 11/6/17.
//

#pragma once

#include <conflagrant/assets/AssetLoader.hh>
#include <conflagrant/assets/Model.hh>

namespace cfl {
namespace assets {
std::shared_ptr<Asset> LoadModel(Path const &path);
} // namespace assets
} // namespace cfl
