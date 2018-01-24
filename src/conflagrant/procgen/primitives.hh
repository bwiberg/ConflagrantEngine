#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/assets/Mesh.hh>
#include <conflagrant/geometry.hh>
#include "MeshDraft.hh"
#include "TriMeshDraft.hh"
#include "QuadMeshDraft.hh"

namespace cfl {
namespace procgen {
using assets::Mesh;

QuadMeshDraft CreateQuad(vec3 const &origin = vec3(0),
                         vec3 const &width = vec3(1, 0, 0),
                         vec3 const &height = vec3(0, 0, 1));

QuadMeshDraft CreatePlane(uvec2 const numQuads,
                          vec3 const &origin = vec3(0),
                          vec2 const &dimensions = vec2(1, 1));
} // namespace procgen
} // namespace cfl
