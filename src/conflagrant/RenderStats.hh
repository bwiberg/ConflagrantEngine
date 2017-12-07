#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
struct RenderStats {
    size_t numVertices{0};
    size_t numTriangles{0};
    size_t numMeshes{0};
    size_t numPointLights{0};
    size_t numDirectionalLights{0};
};
} // namespace cfl
