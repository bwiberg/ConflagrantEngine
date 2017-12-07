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
#include <conflagrant/RenderStats.hh>
#include <conflagrant/Time.hh>

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
                          mat4 &P) {
    $
    using entityx::ComponentHandle;

    ComponentHandle<comp::PerspectiveCamera> perspective;
    ComponentHandle<comp::OrthographicCamera> orthographic;

    GetActiveCamera(entities, outTransform, perspective, orthographic);

    if (perspective) {
        P = perspective->GetProjection();
        frustum = perspective->GetFrustum();
    } else if (orthographic) {
        P = orthographic->GetProjection();
        frustum = orthographic->GetFrustum();
    } else {
        LOG_ERROR(cfl::ForwardRenderer::SetCameraMatrices)
                << "No OrthographicCamera or PerspectiveCamera." << std::endl;
        P = mat4(1);
    }
}

template<bool UseShadows = false>
void UploadPointLights(entityx::EntityManager &entities, gl::Shader &shader, RenderStats &renderStats);

template<bool UseShadows = true>
void UploadDirectionalLights(entityx::EntityManager &entities, gl::Shader &shader, RenderStats &renderStats);

void RenderFullscreenQuad();

void RenderSkydomes(entityx::EntityManager &entities,
                    gl::Shader &shader, GLenum const nextTextureUnit,
                    RenderStats &renderStats, mat4 const &P, mat4 const &V);

template<bool UseDiffuse = true, bool UseSpecular = true, bool UseNormal = true, bool UseShininess = true>
void RenderModels(entityx::EntityManager &entities, gl::Shader &shader,
                  GLenum const nextTextureUnit, RenderStats &renderStats);

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

        ilight++;
        ss.str("");
    }
    shader.Uniform("numPointLights", ilight);
    renderStats.numPointLights = static_cast<size_t>(ilight);
}

template<bool UseShadows = true>
inline void UploadDirectionalLights(entityx::EntityManager &entities,
                                    gl::Shader &shader, gl::Shader &lightpassShader, GLenum &nextTextureUnit,
                                    RenderStats &renderStats) {
    int ilight = 0;
    std::stringstream ss;
    entityx::ComponentHandle<comp::DirectionalLight> light;
    entityx::ComponentHandle<comp::DirectionalLightShadow> shadow;
    entityx::ComponentHandle<comp::OrthographicCamera> camera;

    for (auto entity : entities.entities_with_components(light)) {
        ss << "directionalLights" << "[" << ilight << "]" << ".";
        string const prefix = ss.str();

        float const phi = glm::radians(light->horizontal);
        float const theta = glm::radians(90 - light->vertical);
        vec3 direction(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));

        shader.Uniform(prefix + "direction", direction);
        shader.Uniform(prefix + "intensity", light->intensity);
        shader.Uniform(prefix + "color", light->color);

        if (UseShadows && light->castShadows) {
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

            camera->Size(uvec2(shadow->width, shadow->height));

            mat4 lightV = glm::lookAt(shadow->distanceFromScene * direction, vec3(0.f), geometry::Up);
            mat4 lightP = camera->GetProjection();

            shadow->framebuffer->Bind();
            OGL(glViewport(0, 0, static_cast<GLsizei>(shadow->width),
                           static_cast<GLsizei>(shadow->height)));
            OGL(glClear(GL_DEPTH_BUFFER_BIT));

            // setup shader and V/P matrices
            lightpassShader.Bind();
            lightpassShader.Uniform("time", static_cast<float>(Time::CurrentTime()));
            lightpassShader.Uniform("P", lightP);
            lightpassShader.Uniform("V", lightV);

            OGL(glDisable(GL_CULL_FACE));

            {
                DOLLAR("Shadowmap: Render entities with Model")
                RenderModels<true, false, false, false>(entities, lightpassShader, 0, renderStats);
            }

            shadow->framebuffer->Unbind();

            // feed uniforms
            shader.Bind();
            shader.Uniform(prefix + "hasShadowMap", 1);
            shader.Texture(prefix + "shadowMap", nextTextureUnit++, *shadow->depthTexture);
            shader.Uniform(prefix + "VP", lightP * lightV);
        } else {
            shader.Uniform(prefix + "hasShadowMap", 0);
        }

        ilight++;
        ss.str("");
    }
    shader.Uniform("numDirectionalLights", ilight);
    renderStats.numDirectionalLights = static_cast<size_t>(ilight);
}

inline void RenderFullscreenQuad() {
    auto quadModel = assets::AssetManager::LoadAsset<assets::Model>("fullscreen_quad.obj");
    if (!quadModel) {
        LOG_INFO(cfl::syst::ForwardRenderer::update) << "fullscreen_quad.obj failed to load";
        return;
    }

    OGL(glDisable(GL_CULL_FACE));

    auto &mesh = quadModel->parts[0].first;
    auto &mmesh = *mesh;

    if (mesh->glMeshNeedsUpdate) {
        mesh->UploadToGL();
        mesh->glMeshNeedsUpdate = false;
    }

    mesh->glMesh->DrawElements();
}

inline void RenderSkydomes(entityx::EntityManager &entities,
                           gl::Shader &shader, GLenum const nextTextureUnit,
                           RenderStats &renderStats, mat4 const &P, mat4 const &V) {
    mat4 MVP;

    entityx::ComponentHandle<comp::Skydome> skydome;
    for (auto entity : entities.entities_with_components(skydome)) {
        MVP = P * V * glm::rotate(glm::degrees(skydome->rotationDegrees), geometry::Up);

        shader.Texture("skydomeColor", nextTextureUnit, skydome->texture->texture);
        shader.Uniform("radius", skydome->radius);
        shader.Uniform("MVP", MVP);

        auto &mesh = *skydome->mesh;
        if (mesh.glMeshNeedsUpdate) {
            mesh.UploadToGL();
            mesh.glMeshNeedsUpdate = false;
        }

        mesh.glMesh->DrawElements();

        renderStats.numMeshes++;
        renderStats.numTriangles += mesh.triangles.size();
        renderStats.numVertices += mesh.vertices.size();
    }
}

template<bool UseDiffuse = true, bool UseSpecular = true, bool UseNormal = true, bool UseShininess = true>
inline void RenderModels(entityx::EntityManager &entities,
                         gl::Shader &shader, GLenum const nextTextureUnit,
                         RenderStats &renderStats) {
    static constexpr bool UseMaterial = UseDiffuse || UseSpecular || UseNormal || UseShininess;
    static constexpr bool RenderMesh = true;

    entityx::ComponentHandle<comp::Transform> transform;
    entityx::ComponentHandle<comp::Model> model;

    shader.Bind();

    for (auto entity : entities.entities_with_components(transform, model)) {
        shader.Uniform("M", transform->GetMatrix());

        for (auto const &part : model->value->parts) {
            if (UseMaterial) {
                string const prefix = "material.";
                auto const &material = *part.second;
                auto textureCount = nextTextureUnit;

                if (UseDiffuse) {
                    string const diffusePrefix = prefix + "diffuse.";
                    shader.Uniform(diffusePrefix + "color", material.diffuseColor);

                    int hasDiffuseMap = (material.diffuseTexture != nullptr) ? 1 : 0;
                    shader.Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
                    if (hasDiffuseMap == 1) {
                        shader.Texture(diffusePrefix + "map",
                                       textureCount++,
                                       material.diffuseTexture->texture);
                    }
                }

                if (UseSpecular) {
                    string const specularPrefix = prefix + "specular.";
                    shader.Uniform(specularPrefix + "color", material.specularColor);

                    int hasSpecularMap = (material.specularTexture != nullptr) ? 1 : 0;
                    shader.Uniform(specularPrefix + "hasMap", hasSpecularMap);
                    if (hasSpecularMap == 1) {
                        shader.Texture(specularPrefix + "map",
                                       textureCount++,
                                       material.specularTexture->texture);
                    }
                }

                if (UseNormal) {
                    int hasNormalMap = (material.normalTexture != nullptr) ? 1 : 0;
                    shader.Uniform(prefix + "hasNormalMap", hasNormalMap);
                    if (hasNormalMap == 1) {
                        shader.Texture(prefix + "normalMap",
                                       textureCount++,
                                       material.normalTexture->texture);
                    }
                }

                if (UseShininess) {
                    shader.Uniform(prefix + "shininess", material.shininess);
                }
            }

            if (RenderMesh) {
                auto &mesh = *part.first;
                if (mesh.glMeshNeedsUpdate) {
                    mesh.UploadToGL();
                    mesh.glMeshNeedsUpdate = false;
                }

                mesh.glMesh->DrawElements();

                renderStats.numMeshes++;
                renderStats.numTriangles += mesh.triangles.size();
                renderStats.numVertices += mesh.vertices.size();
            }
        }
    }

    shader.Unbind();
}
} // namespace cfl
