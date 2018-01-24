#include "ProceduralGenerator.hh"

#include <conflagrant/components/Material.hh>
#include <conflagrant/components/Mesh.hh>
#include <conflagrant/components/ProceduralGeometry.hh>

#include <conflagrant/procgen/primitives.hh>
#include <conflagrant/procgen/asset.hh>
#include <conflagrant/procgen/coordinate_transforms.hh>

#include <conflagrant/procgen/graph/operation.hh>
#include <conflagrant/procgen/graph/Polygon.hh>
#include <conflagrant/procgen/graph/operations/Split.hh>
#include <conflagrant/procgen/graph/operations/Extrude.hh>
#include <conflagrant/procgen/graph/operations/Inset.hh>

namespace cfl {
namespace syst {
void ProceduralGenerator::update(entityx::EntityManager &entities, entityx::EventManager &events,
                                 entityx::TimeDelta dt) {
    using entityx::ComponentHandle;

    ComponentHandle<comp::Mesh> mesh;
    ComponentHandle<comp::ProceduralGeometry> procedural;

    for (auto entity : entities.entities_with_components(procedural)) {
        if (!entity.has_component<comp::Mesh>()) {
            mesh = entity.assign<comp::Mesh>();
        } else {
            mesh = entity.component<comp::Mesh>();
        }

        if (!procedural->hasChanged) {
            continue;
        }

        procedural->hasChanged = false;

        mesh->value = (*procedural->generator)(procedural->parameters);
    }
}

bool ProceduralGenerator::DrawWithImGui(ProceduralGenerator &sys, InputManager const &input) {
    return false;
}
} // namespace syst
} // namespace cfl
