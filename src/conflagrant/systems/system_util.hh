#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/ActiveCamera.hh>
#include <conflagrant/components/PerspectiveCamera.hh>
#include <conflagrant/components/OrthographicCamera.hh>
#include <conflagrant/components/Name.hh>
#include <conflagrant/components/Guid.hh>
#include <conflagrant/components/PointLight.hh>
#include <conflagrant/components/DirectionalLight.hh>

#include <entityx/Entity.h>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Skydome.hh>

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
        P = perspective->GetProjection();
    } else if (orthographic) {
        P = orthographic->GetProjection();
    } else {
        LOG_ERROR(cfl::ForwardRenderer::SetCameraMatrices)
                << "No OrthographicCamera or PerspectiveCamera." << std::endl;
        P = mat4(1);
    }
}

//inline void ForEachPointLight(entityx::EntityManager &entities,
//                              std::function<
//                                      entityx::Entity,
//                                      entityx::ComponentHandle<comp::Transform>,
//                                      entityx::ComponentHandle<comp::PointLight>
//                                              (void)> func) {
//    entityx::ComponentHandle<comp::Transform> transform;
//    entityx::ComponentHandle<comp::PointLight> pointLight;
//    for (auto entity : entities.entities_with_components(transform, pointLight)) {
//        func(entity, transform, pointLight);
//    }
//}
//
//inline void ForEachDirectionalLight(entityx::EntityManager &entities,
//                                    std::function<
//                                            entityx::Entity,
//                                            entityx::ComponentHandle<comp::Transform>,
//                                            entityx::ComponentHandle<comp::DirectionalLight>
//                                                    (void)> func) {
//    entityx::ComponentHandle<comp::Transform> transform;
//    entityx::ComponentHandle<comp::DirectionalLight> directionalLight;
//    for (auto entity : entities.entities_with_components(transform, directionalLight)) {
//        func(entity, transform, directionalLight);
//    }
//}
//
//inline void ForEachModel(entityx::EntityManager &entities,
//                         std::function<
//                                 entityx::Entity,
//                                 entityx::ComponentHandle<comp::Transform>,
//                                 entityx::ComponentHandle<comp::Model>
//                                         (void)> func) {
//    entityx::ComponentHandle<comp::Transform> transform;
//    entityx::ComponentHandle<comp::Model> model;
//    for (auto entity : entities.entities_with_components(transform, model)) {
//        func(entity, transform, model);
//    }
//}
//
//inline void ForEachModelPart(comp::Model &model,
//                             std::function<assets::Mesh &, assets::Material &(void)> func) {
//    for (auto &part : model.value->parts) {
//        func(*part.first, *part.second);
//    }
//}
//
//inline void ForEachSkydome(entityx::EntityManager &entities,
//                           std::function<
//                                   entityx::Entity,
//                                   entityx::ComponentHandle<comp::Skydome>(void)> func) {
//    entityx::ComponentHandle<comp::Skydome> skydome;
//    for (auto entity : entities.entities_with_components(skydome)) {
//        func(entity, skydome);
//    }
//}
} // namespace cfl
