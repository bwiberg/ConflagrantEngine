#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/AssetLoader.hh>
#include <conflagrant/geometry/Mesh.hh>

#include <assimp/Importer.hpp>

namespace cfl {
namespace assets {
namespace loaders {

using geometry::Mesh;

std::shared_ptr<Mesh> LoadMesh(Path const &path) {
    if (!path.exists())
        return nullptr;


}
}
}
} // namespace cfl
