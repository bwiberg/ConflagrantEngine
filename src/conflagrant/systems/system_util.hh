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
#include <conflagrant/components/DirectionalLightShadow.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Skydome.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/gl/State.hh>
#include <conflagrant/RenderStats.hh>
#include <conflagrant/Time.hh>
#include <conflagrant/components/BoundingSphere.hh>
#include <conflagrant/components/VctProperties.hh>
#include <conflagrant/math.hh>

#include <entityx/Entity.h>

namespace cfl {
inline entityx::Entity GetActiveCamera(entityx::EntityManager &entities,
                                       entityx::ComponentHandle<comp::Transform> &outTransform,
                                       entityx::ComponentHandle<comp::PerspectiveCamera> &outPerspective,
                                       entityx::ComponentHandle<comp::OrthographicCamera> &outOrtographic) {
    $

    entityx::ComponentHandle<comp::ActiveCamera> active;

    // 1. Check if we have any Perspective- or OrthographicCamera's with ActiveCamera attached
    for (auto entity : entities.entities_with_components(outTransform, active, outPerspective))
        return entity;
    for (auto entity : entities.entities_with_components(outTransform, active, outOrtographic))
        return entity;

    // 2. Check if we have any Perspective- or OrthographicCamera's without ActiveCamera attached
    //    => if so, attach ActiveCamera and return
    for (auto entity : entities.entities_with_components(outTransform, outPerspective)) {
        entity.assign<comp::ActiveCamera>();
        return entity;
    }
    for (auto entity : entities.entities_with_components(outTransform, outOrtographic)) {
        entity.assign<comp::ActiveCamera>();
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
                          geometry::Frustum &frustum,
                          mat4 &P, float &zNear, float &zFar) {
    $
    using entityx::ComponentHandle;

    ComponentHandle<comp::PerspectiveCamera> perspective;
    ComponentHandle<comp::OrthographicCamera> orthographic;

    GetActiveCamera(entities, outTransform, perspective, orthographic);

    if (perspective) {
        P = perspective->GetProjection();
        frustum = perspective->GetFrustum();
        zNear = perspective->ZNear();
        zFar = perspective->ZFar();

    } else if (orthographic) {
        P = orthographic->GetProjection();
        frustum = orthographic->GetFrustum();
        zNear = orthographic->ZNear();
        zFar = orthographic->ZFar();

    } else {
        LOG_ERROR(cfl::ForwardRenderer::SetCameraMatrices)
                << "No OrthographicCamera or PerspectiveCamera." << std::endl;
        P = mat4(1);
    }
}

template<bool UseShadows = false>
void UploadPointLights(entityx::EntityManager &entities, gl::Shader &shader, RenderStats &renderStats);

void RenderPointLightShadows(entityx::EntityManager &entities,
                             gl::Shader &lightpassShader, GLenum const nextTextureUnit,
                             RenderStats &renderStats, bool cullModelsAndMeshes = false);

template<bool UseShadows = true>
void UploadDirectionalLights(entityx::EntityManager &entities,
                             gl::Shader &shader, gl::Shader &lightpassShader, GLenum &nextTextureUnit,
                             RenderStats &renderStats, bool cullModelsAndMeshes = false);

void RenderFullscreenQuad(RenderStats &renderStats);

void RenderFullscreenTriangle(RenderStats &renderStats);

void RenderUnitSphere(float radius, RenderStats &renderStats);

void RenderBoundingSpheres(entityx::EntityManager &entities,
                           gl::Shader &shader, GLenum const nextTextureUnit,
                           RenderStats &renderStats,
                           float wireframe = true, geometry::Frustum const *frustum = nullptr);

void RenderSkydomes(entityx::EntityManager &entities,
                    gl::Shader &shader, GLenum const nextTextureUnit,
                    RenderStats &renderStats, mat4 const &P, mat4 const &V);

template<bool UseDiffuse = true, bool UseSpecular = true, bool UseNormal = true, bool UseShininess = true>
void RenderModel(comp::Transform &transform, comp::Model &model,
                 comp::VctProperties const *vctProperties,
                 gl::Shader &shader, GLenum const nextTextureUnit,
                 RenderStats &renderStats, geometry::Frustum const *frustum = nullptr);

template<bool UseDiffuse = true, bool UseSpecular = true, bool UseNormal = true, bool UseShininess = true>
void RenderModels(entityx::EntityManager &entities, gl::Shader &shader,
                  GLenum const nextTextureUnit, RenderStats &renderStats, geometry::Frustum const *frustum = nullptr);

template<bool UseShadows = false>
inline void UploadPointLights(entityx::EntityManager &entities,
                              gl::Shader &shader,
                              RenderStats &renderStats) {
    entityx::ComponentHandle<comp::Transform> transform;
    entityx::ComponentHandle<comp::PointLight> pointLight;

    int ilight = 0;
    std::stringstream ss;
    for (auto const &entity : entities.entities_with_components(transform, pointLight)) {
        ss << "pointLights" << "[" << ilight << "]" << ".";
        string const prefix = ss.str();

        shader.Uniform(prefix + "worldPosition", transform->Position());
        shader.Uniform(prefix + "intensity", pointLight->intensity);
        shader.Uniform(prefix + "color", pointLight->color);
        renderStats.UniformCalls += 3;

        ilight++;
        ss.str("");
    }
    shader.Uniform("numPointLights", ilight);
    renderStats.UniformCalls++;
    renderStats.PointLights = static_cast<size_t>(ilight);
}

inline void RenderDirectionalLightShadows(entityx::EntityManager &entities,
                                          gl::Shader &lightpassShader,
                                          RenderStats &renderStats, bool cullModelsAndMeshes) {
    entityx::ComponentHandle<comp::DirectionalLight> light;
    entityx::ComponentHandle<comp::DirectionalLightShadow> shadow;
    entityx::ComponentHandle<comp::OrthographicCamera> camera;

    for (auto entity : entities.entities_with_components(light)) {
        float const phi = glm::radians(light->horizontal);
        float const theta = glm::radians(90 - light->vertical);
        vec3 direction(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));

        if (light->castShadows) {
            if (!entity.has_component<comp::DirectionalLightShadow>()) {
                shadow = entity.assign<comp::DirectionalLightShadow>();
            } else {
                shadow = entity.component<comp::DirectionalLightShadow>();
            }

            if (!entity.has_component<comp::OrthographicCamera>()) {
                camera = entity.assign<comp::OrthographicCamera>();
            } else {
                camera = entity.component<comp::OrthographicCamera>();
            }

            if (shadow->hasChanged) {
                shadow->Reset();
                shadow->hasChanged = false;
            }

            // camera->Size(uvec2(shadow->width, shadow->height));

            mat4 lightV = glm::lookAt(shadow->distanceFromScene * direction, vec3(0.f), geometry::Up);
            auto const &lightP = camera->GetProjection();
            auto const &frustum = camera->GetFrustum();

            auto const transformedFrustum = glm::inverse(lightV) * frustum;

            shadow->framebuffer->Bind();
            OGL(glViewport(0, 0, static_cast<GLsizei>(shadow->width),
                           static_cast<GLsizei>(shadow->height)));
            OGL(glClear(GL_DEPTH_BUFFER_BIT));

            // setup shader and V/P matrices
            lightpassShader.Bind();
            lightpassShader.Uniform("time", static_cast<float>(Time::CurrentTime()));
            lightpassShader.Uniform("P", lightP);
            lightpassShader.Uniform("V", lightV);
            renderStats.UniformCalls += 3;

            auto scopedState = gl::ScopedState()
                    .Disable(GL_CULL_FACE)
                    .Build();

            {
                DOLLAR("Shadowmap: Render entities with Model")
                if (cullModelsAndMeshes) {
                    RenderModels<true, false, false, false>(entities, lightpassShader, 0, renderStats,
                                                            &transformedFrustum);
                } else {
                    RenderModels<true, false, false, false>(entities, lightpassShader, 0, renderStats);
                }
            }

            shadow->framebuffer->Unbind();
        }
    }
}

template<bool UseShadows = true>
inline void UploadDirectionalLights(entityx::EntityManager &entities,
                                    gl::Shader &shader, GLenum &nextTextureUnit,
                                    RenderStats &renderStats, bool cullModelsAndMeshes) {
    int ilight = 0;
    std::stringstream ss;
    entityx::ComponentHandle<comp::DirectionalLight> light;
    entityx::ComponentHandle<comp::DirectionalLightShadow> shadow;
    entityx::ComponentHandle<comp::OrthographicCamera> camera;

    auto const nextTextureUnitStart = nextTextureUnit;

    for (auto entity : entities.entities_with_components(light)) {
        auto localNextTextureUnit = nextTextureUnitStart;

        ss << "directionalLights" << "[" << ilight << "]" << ".";
        string const prefix = ss.str();

        float const phi = glm::radians(light->horizontal);
        float const theta = glm::radians(90 - light->vertical);
        vec3 direction(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));

        shader.Uniform(prefix + "direction", direction);
        shader.Uniform(prefix + "intensity", light->intensity);
        shader.Uniform(prefix + "color", light->color);
        renderStats.UniformCalls += 3;

        shadow = entity.component<comp::DirectionalLightShadow>();
        camera = entity.component<comp::OrthographicCamera>();
        if (UseShadows && light->castShadows && (shadow && camera)) {
            mat4 lightV = glm::lookAt(shadow->distanceFromScene * direction, vec3(0.f), geometry::Up);
            auto const &lightP = camera->GetProjection();

            // feed uniforms
            shader.Uniform(prefix + "hasShadowMap", 1);
            shader.Texture(prefix + "shadowMap", localNextTextureUnit++, *shadow->depthTexture);
            shader.Uniform(prefix + "VP", lightP * lightV);
            renderStats.UniformCalls += 3;
        } else {
            shader.Uniform(prefix + "hasShadowMap", 0);
            renderStats.UniformCalls += 1;
        }

        ilight++;
        ss.str("");
        nextTextureUnit = localNextTextureUnit;
    }

    shader.Uniform("numDirectionalLights", ilight);
    renderStats.UniformCalls++;
    renderStats.DirectionalLights = static_cast<size_t>(ilight);
}

inline void RenderFullscreenQuad(RenderStats &renderStats) {
    auto quadModel = assets::AssetManager::LoadAsset<assets::Model>("fullscreen_quad.obj");
    if (!quadModel) {
        LOG_ERROR(cfl::RenderFullscreenQuad) << "fullscreen_quad.obj failed to load";
        return;
    }

    auto scopedState = gl::ScopedState()
            .Disable(GL_CULL_FACE)
            .Build();

    auto &mesh = quadModel->parts[0].first;

    if (mesh->needsUpdate) {
        mesh->Update();
        mesh->needsUpdate = false;
    }

    mesh->glMesh->DrawElements();
    renderStats.DrawCalls++;
}

inline void RenderFullscreenTriangle(RenderStats &renderStats) {
    auto quadModel = assets::AssetManager::LoadAsset<assets::Model>("fullscreen_triangle.obj");
    if (!quadModel) {
        LOG_ERROR(cfl::RenderFullscreenQuad) << "fullscreen_triangle.obj failed to load";
        return;
    }

    auto scopedState = gl::ScopedState()
            .Disable(GL_CULL_FACE)
            .Build();

    auto &mesh = quadModel->parts[0].first;

    if (mesh->needsUpdate) {
        mesh->Update();
        mesh->needsUpdate = false;
    }

    mesh->glMesh->DrawElements();
    renderStats.DrawCalls++;
}

inline void RenderUnitSphere(float radius, RenderStats &renderStats) {
    static constexpr auto RadiusToIcosphereLevelFactor = 0.025f;

    auto icosphereLevel = math::Clamp(static_cast<int>(radius * RadiusToIcosphereLevelFactor),
                                      2, 5);

    std::stringstream ss;
    ss << "icosphere" << icosphereLevel << ".obj";

    auto sphereModel = assets::AssetManager::LoadAsset<assets::Model>(ss.str());
    if (!sphereModel) {
        LOG_ERROR(cfl::RenderUnitSphere) << ss.str() << " failed to load";
        return;
    }

    auto &mesh = sphereModel->parts[0].first;

    if (mesh->needsUpdate) {
        mesh->Update();
        mesh->needsUpdate = false;
    }

    mesh->glMesh->DrawElements();
    renderStats.DrawCalls++;
}

inline void RenderBoundingSpheres(entityx::EntityManager &entities,
                                  gl::Shader &shader, GLenum const nextTextureUnit,
                                  RenderStats &renderStats,
                                  float wireframe, geometry::Frustum const *frustum) {
    auto scopedStateBuilder = gl::ScopedState();

    if (wireframe) {
        scopedStateBuilder.PolygonModeFrontBack(GL_LINE, GL_LINE);
    }

    auto scopedState = scopedStateBuilder.Build();

    entityx::ComponentHandle<comp::Transform> transform;
    entityx::ComponentHandle<comp::BoundingSphere> boundingSphere;
    entityx::ComponentHandle<comp::Model> model;

    string const diffusePrefix = "material.diffuse.";
    shader.Uniform(diffusePrefix + "color", vec3(1));
    shader.Uniform(diffusePrefix + "hasMap", 0);
    renderStats.UniformCalls += 2;

    for (auto entity : entities.entities_with_components(transform, boundingSphere)) {
        if (frustum &&
            frustum->ComputeIntersection(
                    geometry::Transform(boundingSphere->sphere, transform->GetMatrix(), transform->Scale())) ==
            geometry::IntersectionType::OUTSIDE) {
            continue;
        }

        auto const M = transform->GetMatrix()
                       * glm::translate(boundingSphere->sphere.center)
                       * glm::scale(vec3(boundingSphere->sphere.radius));
        shader.Uniform("M", M);
        renderStats.UniformCalls++;

        RenderUnitSphere(boundingSphere->sphere.radius, renderStats);
    }

    for (auto entity : entities.entities_with_components(transform, model)) {
        if (!model->value || model->value->parts.size() < 2) {
            // the model's single bounding sphere is exactly the same as the entire model's bounding sphere
            // ==> skip it
            continue;
        }

        for (auto const &part : model->value->parts) {
            auto &mesh = *part.first;
            if (mesh.needsUpdate) {
                mesh.Update();
                mesh.needsUpdate = false;
            }

            if (frustum &&
                frustum->ComputeIntersection(
                        geometry::Transform(boundingSphere->sphere, transform->GetMatrix(), transform->Scale())) ==
                geometry::IntersectionType::OUTSIDE) {
                continue;
            }

            auto const M = transform->GetMatrix()
                           * glm::translate(mesh.boundingSphere.center)
                           * glm::scale(vec3(mesh.boundingSphere.radius));
            shader.Uniform("M", M);
            renderStats.UniformCalls++;

            RenderUnitSphere(mesh.boundingSphere.radius, renderStats);
        }
    }
}


inline void RenderSkydomes(entityx::EntityManager &entities,
                           gl::Shader &shader, GLenum const nextTextureUnit,
                           RenderStats &renderStats, mat4 const &P, mat4 const &V) {
    mat4 MVP;

    entityx::ComponentHandle<comp::Skydome> skydome;
    for (auto entity : entities.entities_with_components(skydome)) {
        MVP = P * V * glm::rotate(glm::radians(skydome->rotationDegrees), geometry::Up);

        shader.Texture("skydomeColor", nextTextureUnit, skydome->texture->texture);
        shader.Uniform("radius", skydome->radius);
        shader.Uniform("MVP", MVP);
        renderStats.UniformCalls += 3;

        auto &mesh = *skydome->mesh;
        if (mesh.needsUpdate) {
            mesh.Update();
            mesh.needsUpdate = false;
        }

        mesh.glMesh->DrawElements();
        renderStats.DrawCalls++;

        renderStats.MeshesRendered++;
        renderStats.Triangles += mesh.triangles.size();
        renderStats.Vertices += mesh.vertices.size();
    }
}

template<bool UseDiffuse = true, bool UseSpecular = true, bool UseNormal = true, bool UseShininess = true>
inline void RenderModel(comp::Transform &transform, comp::Model &model,
                        comp::VctProperties const *vctProperties,
                        gl::Shader &shader, GLenum const nextTextureUnit,
                        RenderStats &renderStats, geometry::Frustum const *frustum) {
    static constexpr bool UseMaterial = UseDiffuse || UseSpecular || UseNormal || UseShininess;
    static constexpr bool RenderMesh = true;

    if (!model.value) {
        return;
    }

    auto const &M = transform.GetMatrix();
    shader.Uniform("M", M);
    renderStats.UniformCalls++;

    for (auto const &part : model.value->parts) {
        if (UseMaterial) {
            string const prefix = "material.";
            auto const &material = *part.second;
            auto textureCount = nextTextureUnit;

            if (UseDiffuse) {
                string const diffusePrefix = prefix + "diffuse.";
                int hasDiffuseMap = (material.diffuseTexture != nullptr) ? 1 : 0;

                shader.Uniform(diffusePrefix + "color", material.diffuseColor);
                shader.Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
                renderStats.UniformCalls += 3;

                if (hasDiffuseMap == 1) {
                    shader.Texture(diffusePrefix + "map",
                                   textureCount++,
                                   material.diffuseTexture->texture);
                    renderStats.UniformCalls++;
                }

            }

            if (UseSpecular) {
                string const specularPrefix = prefix + "specular.";
                int hasSpecularMap = (material.specularTexture != nullptr) ? 1 : 0;

                shader.Uniform(specularPrefix + "color", material.specularColor);
                shader.Uniform(specularPrefix + "hasMap", hasSpecularMap);
                renderStats.UniformCalls += 2;

                if (hasSpecularMap == 1) {
                    shader.Texture(specularPrefix + "map",
                                   textureCount++,
                                   material.specularTexture->texture);
                    renderStats.UniformCalls++;
                }
            }

            if (UseNormal) {
                int hasNormalMap = (material.normalTexture != nullptr) ? 1 : 0;
                shader.Uniform(prefix + "hasNormalMap", hasNormalMap);
                renderStats.UniformCalls++;

                if (hasNormalMap == 1) {
                    shader.Texture(prefix + "normalMap",
                                   textureCount++,
                                   material.normalTexture->texture);
                    renderStats.UniformCalls++;
                }
            }

            if (UseShininess) {
                shader.Uniform(prefix + "shininess", material.shininess);
                renderStats.UniformCalls++;
            }

            if (vctProperties) {
                shader.Uniform(prefix + "radiance", vctProperties->radiance);
                shader.Uniform(prefix + "specular.color", vec3(vctProperties->specularReflectance));
                shader.Uniform(prefix + "specular.hasMap", 0);
                if (material.specularTexture != nullptr) {
                    textureCount--;
                }

                renderStats.UniformCalls += 2;
            } else {
                shader.Uniform(prefix + "radiance", 0.0f);
                renderStats.UniformCalls++;
            }

        }

        if (RenderMesh) {
            auto &mesh = *part.first;
            if (mesh.needsUpdate) {
                mesh.Update();
                mesh.needsUpdate = false;
            }

            if (frustum &&
                frustum->ComputeIntersection(geometry::Transform(mesh.boundingSphere, M, transform.Scale())) ==
                geometry::IntersectionType::OUTSIDE) {
                renderStats.MeshesCulled++;
                continue;
            }

            mesh.glMesh->DrawElements();
            renderStats.DrawCalls++;

            renderStats.MeshesRendered++;
            renderStats.Triangles += mesh.triangles.size();
            renderStats.Vertices += mesh.vertices.size();
        }
    }
};

template<bool UseDiffuse = true, bool UseSpecular = true, bool UseNormal = true, bool UseShininess = true>
inline void RenderModels(entityx::EntityManager &entities,
                         gl::Shader &shader, GLenum const nextTextureUnit,
                         RenderStats &renderStats, geometry::Frustum const *frustum) {
    entityx::ComponentHandle<comp::Transform> transform;
    entityx::ComponentHandle<comp::Model> model;

    shader.Bind();

    for (auto entity : entities.entities_with_components(transform, model)) {
        if (frustum) {
            auto boundingSphere = entity.component<comp::BoundingSphere>();
            if (!boundingSphere) {
                boundingSphere = entity.assign<comp::BoundingSphere>();
                boundingSphere->Reset(*model);
            }

            auto const &M = transform->GetMatrix();
            auto intersection = frustum->ComputeIntersection(
                    geometry::Transform(boundingSphere->sphere, M, transform->Scale()));

            if (intersection == geometry::IntersectionType::OUTSIDE) {
                renderStats.ModelsCulled++;
                renderStats.MeshesCulled += (model->value ? model->value->parts.size() : 0);
                continue;
            }
        }

        comp::VctProperties *vctProperties = nullptr;
        auto c = entity.component<comp::VctProperties>();
        if (c) {
            vctProperties = &(*c);
        }

        RenderModel<UseDiffuse, UseSpecular, UseNormal, UseShininess>(*transform, *model,
                                                                      vctProperties,
                                                                      shader, nextTextureUnit,
                                                                      renderStats, frustum);
        renderStats.ModelsRendered++;
    }

    shader.Unbind();
}
} // namespace cfl
