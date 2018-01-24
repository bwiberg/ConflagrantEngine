#include "primitives.hh"

#include "coordinate_transforms.hh"

namespace cfl {
namespace procgen {
QuadMeshDraft CreateQuad(vec3 const &origin, vec3 const &width, vec3 const &height) {
    QuadMeshDraft draft(4, 1);
    draft.AddQuad(origin, width, height);
    return draft;
}

QuadMeshDraft CreatePlane(uvec2 const numQuads, const vec3 &origin, const vec2 &dimensions) {
#define ZERO 0.f
    QuadMeshDraft draft;

    vec3 const normal(ZERO, 1.f, ZERO);
    vec3 const tangent(1.f, ZERO, ZERO);

    float const dx = dimensions[0] / numQuads[0];
    float const dz = dimensions[1] / numQuads[1];

    uvec2 const numVertices(numQuads.x + 1, numQuads.y + 1);

    // create vertices

    uvec2 index;
    for (index[1] = 0; index[1] < numVertices[1]; ++index[1]) {
        for (index[0] = 0; index[0] < numVertices[0]; ++index[0]) {
            draft.positions.emplace_back(index[0] * dx, ZERO, index[1] * dz);
            draft.normals.emplace_back(normal);
            draft.tangents.emplace_back(tangent);
            draft.uvs.emplace_back(static_cast<float>(index[0]) / (numVertices[0] - 1),
                                   static_cast<float>(index[1]) / (numVertices[1] - 1));
        }
    }

    // create quads

    for (index[1] = 0; index[1] < numVertices[1]; ++index[1]) {
        for (index[0] = 0; index[0] < numVertices[0]; ++index[0]) {
            uint const v0 = Flatten(index + uvec2(0, 0), numVertices);
            uint const v1 = Flatten(index + uvec2(1, 0), numVertices);
            uint const v2 = Flatten(index + uvec2(1, 1), numVertices);
            uint const v3 = Flatten(index + uvec2(0, 1), numVertices);

            draft.polygons.push_back({v0, v1, v2, v3});
        }
    }

    return draft;
#undef ZERO
}
} // namespace procgen
} // namespace cfl
