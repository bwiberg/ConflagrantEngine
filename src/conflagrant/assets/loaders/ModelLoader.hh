//
// Created by bwiberg on 11/6/17.
//

#pragma once

#include <conflagrant/assets/AssetLoader.hh>
#include <conflagrant/Model.hh>

namespace cfl {
namespace assets {
std::shared_ptr<Model> LoadModel(Path const &path, AssetManager &manager);
} // namespace assets
} // namespace cfl
