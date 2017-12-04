#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/ActiveCamera.hh>
#include <conflagrant/components/PerspectiveCamera.hh>
#include <conflagrant/components/OrthographicCamera.hh>
#include <conflagrant/components/Name.hh>
#include <conflagrant/components/Guid.hh>

#include <entityx/Entity.h>

namespace cfl {
inline entityx::Entity GetActiveCamera(entityx::EntityManager &entities,
                                       entityx::ComponentHandle<comp::Transform> &outTransform,
                                       entityx::ComponentHandle<comp::PerspectiveCamera> &outPerspective,
                                       entityx::ComponentHandle<comp::OrthographicCamera> &outOrtographic) {
    $

    entityx::ComponentHandle<comp::ActiveCamera> active;
    for (auto entity : entities.entities_with_components(outTransform, active, outPerspective)) {
        return entity;
    }

    for (auto entity : entities.entities_with_components(outTransform, active, outOrtographic)) {
        return entity;
    }

    entityx::Entity newActiveCamera = entities.create();
    newActiveCamera.assign<comp::Name>()->value = "(auto-created camera)";
    newActiveCamera.assign<comp::Guid>();
    newActiveCamera.assign<comp::ActiveCamera>();

    outTransform = newActiveCamera.assign<comp::Transform>();
    outTransform->Position(vec3(0, 0, 10));

    outPerspective = newActiveCamera.assign<comp::PerspectiveCamera>();

    return newActiveCamera;
}

inline void GetCameraInfo(entityx::EntityManager &entities,
                          entityx::ComponentHandle<comp::Transform> &outTransform,
                          mat4 &P) {
    $
    using entityx::ComponentHandle;

    ComponentHandle<comp::ActiveCamera> active;
    ComponentHandle<comp::PerspectiveCamera> perspective;
    ComponentHandle<comp::OrthographicCamera> orthographic;

    auto activeCamera = GetActiveCamera(entities, outTransform, perspective, orthographic);

    if (perspective) {
        P = perspective->projection;
    } else if (orthographic) {
        P = orthographic->projection;
    } else {
        LOG_ERROR(cfl::ForwardRenderer::SetCameraMatrices)
                << "No OrthographicCamera or PerspectiveCamera." << std::endl;
        P = mat4(1);
    }
}
} // namespace cfl
