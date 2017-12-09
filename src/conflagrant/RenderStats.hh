#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
struct RenderStats {
    size_t numVertices{0};
    size_t numTriangles{0};
    size_t numRenderedModels{0};
    size_t numCulledModels{0};
    size_t numRenderedMeshes{0};
    size_t numCulledMeshes{0};
    size_t numPointLights{0};
    size_t numDirectionalLights{0};
};
} // namespace cfl
