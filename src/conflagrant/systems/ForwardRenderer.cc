#include "ForwardRenderer.hh"
#include "system_util.hh"

#include <conflagrant/systems/CameraController.hh>
#include <conflagrant/Engine.hh>
#include <conflagrant/Time.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/PointLight.hh>
#include <conflagrant/components/DirectionalLight.hh>
#include <conflagrant/components/DirectionalLightShadow.hh>
#include <conflagrant/components/Skydome.hh>

#include <imgui.h>

namespace cfl {
namespace syst {
ForwardRenderer::ForwardRenderer() {
    LoadShaders();
}

void ForwardRenderer::LoadShaders() {
    $
    forwardShader = gl::LoadShader("forward.vert", "forward.frag");
    skydomeShader = gl::LoadShader("forward_skydome.vert", "forward_skydome.frag");
    shadowmapLightpassShader = gl::LoadShader("shadowmap_lightpass.vert", "shadowmap_lightpass.frag");
    shadowmapVisShader = gl::LoadShader("shadowmap_visualization.vert", "shadowmap_visualization.frag");
}

void ForwardRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    $
    renderStats = RenderStats{};

    using entityx::ComponentHandle;
    ComponentHandle<comp::Transform> transform, cameraTransform;
    ComponentHandle<comp::Model> model;
    ComponentHandle<comp::PointLight> pointLight;
    ComponentHandle<comp::DirectionalLight> directionalLight;
    ComponentHandle<comp::Skydome> skydome;

    mat4 P;
    GetCameraInfo(entities, cameraTransform, P);

    mat4 Vinv = glm::inverse(cameraTransform->GetMatrix());

    GLenum forwardShaderTextureCount = 0;

    forwardShader->Bind();

    forwardShader->Uniform("V", Vinv);
    forwardShader->Uniform("P", P);
    forwardShader->Uniform("EyePos", cameraTransform->Position());
    forwardShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

    {
        DOLLAR("Upload PointLight data")
        int ilight = 0;
        std::stringstream ss;
        for (auto const &entity : entities.entities_with_components(transform, pointLight)) {
            ss << "pointLights" << "[" << ilight << "]" << ".";
            string const prefix = ss.str();

            forwardShader->Uniform(prefix + "worldPosition", transform->Position());
            forwardShader->Uniform(prefix + "intensity", pointLight->intensity);
            forwardShader->Uniform(prefix + "color", pointLight->color);

            ilight++;
            ss.str("");
        }
        forwardShader->Uniform("numPointLights", ilight);
        renderStats.numPointLights = static_cast<size_t>(ilight);
    }

    entityx::ComponentHandle<comp::DirectionalLightShadow> directionalLightShadow;

    {
        DOLLAR("Upload DirectionalLight data")
        int ilight = 0;
        std::stringstream ss;
        entityx::ComponentHandle<comp::DirectionalLightShadow> lightShadow;
        entityx::ComponentHandle<comp::OrthographicCamera> lightCamera;

        for (auto entity : entities.entities_with_components(directionalLight)) {
            ss << "directionalLights" << "[" << ilight << "]" << ".";
            string const prefix = ss.str();

            float const phi = glm::radians(directionalLight->horizontal);
            float const theta = glm::radians(90 - directionalLight->vertical);
            vec3 direction(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));

            forwardShader->Uniform(prefix + "direction", direction);
            forwardShader->Uniform(prefix + "intensity", directionalLight->intensity);
            forwardShader->Uniform(prefix + "color", directionalLight->color);

            // shadow mapping
            {
                if (directionalLight->castShadows) {
                    if (!entity.has_component<comp::DirectionalLightShadow>()) {
                        lightShadow = entity.assign<comp::DirectionalLightShadow>();
                    } else {
                        lightShadow = entity.component<comp::DirectionalLightShadow>();
                    }
                    directionalLightShadow = lightShadow;

                    if (!entity.has_component<comp::OrthographicCamera>()) {
                        lightCamera = entity.assign<comp::OrthographicCamera>();
                    } else {
                        lightCamera = entity.component<comp::OrthographicCamera>();
                    }

                    if (lightShadow->hasChanged) {
                        lightShadow->Reset();
                        lightShadow->hasChanged = false;
                    }

                    lightCamera->Size(uvec2(lightShadow->width, lightShadow->height));

                    mat4 V = glm::lookAt(lightShadow->distanceFromScene * direction, vec3(0.f), geometry::Up);
                    mat4 Pshadow = lightCamera->GetProjection();

                    lightShadow->framebuffer->Bind();
                    OGL(glViewport(0, 0, static_cast<GLsizei>(lightShadow->width),
                                   static_cast<GLsizei>(lightShadow->height)));
                    OGL(glClear(GL_DEPTH_BUFFER_BIT));

                    // setup shader and V/P matrices
                    shadowmapLightpassShader->Bind();
                    shadowmapLightpassShader->Uniform("time", static_cast<float>(Time::CurrentTime()));
                    shadowmapLightpassShader->Uniform("P", Pshadow);
                    shadowmapLightpassShader->Uniform("V", V);

                    OGL(glDisable(GL_CULL_FACE));
                    // render scene
                    {
                        DOLLAR("Shadowmap: Render entities with Model")

                        for (auto const &entityRender : entities.entities_with_components(transform, model)) {
                            shadowmapLightpassShader->Uniform("M", transform->GetMatrix());

                            for (auto const &part : model->value->parts) {
                                {
                                    auto const &material = *part.second;

                                    // diffuse texture is neccessary since it contains alpha color
                                    {
                                        string const diffusePrefix = "material.diffuse.";
                                        shadowmapLightpassShader->Uniform(diffusePrefix + "color",
                                                                          material.diffuseColor);

                                        int hasDiffuseMap = (material.diffuseTexture != nullptr) ? 1 : 0;
                                        shadowmapLightpassShader->Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
                                        if (hasDiffuseMap == 1) {
                                            shadowmapLightpassShader->Texture(diffusePrefix + "map", 0,
                                                                              material.diffuseTexture->texture);
                                        }
                                    }
                                }

                                // render mesh
                                {
                                    auto &mesh = *part.first;
                                    if (mesh.glMeshNeedsUpdate) {
                                        mesh.UploadToGL();
                                        mesh.glMeshNeedsUpdate = false;
                                    }

                                    mesh.glMesh->DrawElements();
                                }
                            }
                        }
                    }

                    lightShadow->framebuffer->Unbind();

                    // feed uniforms
                    forwardShader->Bind();
                    forwardShader->Uniform(prefix + "hasShadowMap", 1);
                    forwardShader->Texture(prefix + "shadowMap",
                                           forwardShaderTextureCount++,
                                           *lightShadow->depthTexture);
                    forwardShader->Uniform(prefix + "VP", Pshadow * V);
                } else {
                    forwardShader->Uniform(prefix + "hasShadowMap", 0);
                }
            }

            ilight++;
            ss.str("");
        }
        forwardShader->Uniform("numDirectionalLights", ilight);
        renderStats.numDirectionalLights = static_cast<size_t>(ilight);
    }

    uvec2 size = window->GetSize();
    OGL(glViewport(0, 0, size.x, size.y));
    OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

    static bool renderShadowMap = false;
    ImGui::Begin("Shadow");
    ImGui::Checkbox("Render shadow map", &renderShadowMap);
    ImGui::End();

    if (directionalLightShadow && renderShadowMap) {
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

        shadowmapVisShader->Bind();
        shadowmapVisShader->Texture("shadowMap", 0, *directionalLightShadow->depthTexture);
        mesh->glMesh->DrawElements();
        shadowmapVisShader->Unbind();

        return;
    }

    {
        DOLLAR("Render entities with Model")
        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_BACK));
        OGL(glEnable(GL_DEPTH_TEST));

        for (auto const &entity : entities.entities_with_components(transform, model)) {
            forwardShader->Uniform("M", transform->GetMatrix());

            for (auto const &part : model->value->parts) {
                string const prefix = "material.";

                {
                    auto const &material = *part.second;
                    auto textureCount = forwardShaderTextureCount;

                    {
                        string const diffusePrefix = prefix + "diffuse.";
                        forwardShader->Uniform(diffusePrefix + "color", material.diffuseColor);

                        int hasDiffuseMap = (material.diffuseTexture != nullptr) ? 1 : 0;
                        forwardShader->Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
                        if (hasDiffuseMap == 1) {
                            forwardShader->Texture(diffusePrefix + "map",
                                                   textureCount++,
                                                   material.diffuseTexture->texture);
                        }
                    }

                    {
                        string const specularPrefix = prefix + "specular.";
                        forwardShader->Uniform(specularPrefix + "color", material.specularColor);

                        int hasSpecularMap = (material.specularTexture != nullptr) ? 1 : 0;
                        forwardShader->Uniform(specularPrefix + "hasMap", hasSpecularMap);
                        if (hasSpecularMap == 1) {
                            forwardShader->Texture(specularPrefix + "map",
                                                   textureCount++,
                                                   material.specularTexture->texture);
                        }
                    }

                    {
                        int hasNormalMap = (material.normalTexture != nullptr) ? 1 : 0;
                        forwardShader->Uniform(prefix + "hasNormalMap", hasNormalMap);
                        if (hasNormalMap == 1) {
                            forwardShader->Texture(prefix + "normalMap",
                                                   textureCount++,
                                                   material.normalTexture->texture);
                        }
                    }

                    forwardShader->Uniform(prefix + "shininess", material.shininess);
                }

                // render mesh
                {
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
    }

    forwardShader->Unbind();

    Vinv = glm::inverse(glm::toMat4(cameraTransform->Quaternion()));

    skydomeShader->Bind();
    forwardShader->Uniform("EyePos", cameraTransform->Position());
    forwardShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

    {
        DOLLAR("Render entities with Skydome component")
        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_FRONT));
        OGL(glEnable(GL_DEPTH_TEST));

        mat4 MVP;

        for (auto const &entity : entities.entities_with_components(skydome)) {
            MVP = P * Vinv * glm::rotate(glm::degrees(skydome->rotationDegrees), geometry::Up);

            skydomeShader->Texture("skydomeColor", 0, skydome->texture->texture);
            skydomeShader->Uniform("radius", skydome->radius);
            skydomeShader->Uniform("MVP", MVP);

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

    skydomeShader->Unbind();
}

bool ForwardRenderer::DrawWithImGui(ForwardRenderer &sys, InputManager const &input) {
    $
    if (ImGui::Button("Reload shaders")) {
        sys.LoadShaders();
    }

    int swapInterval = sys.window->GetSwapInterval();
    string currentRenderMode = (swapInterval == 0) ? "Enable VSync" : "Disable VSync";
    if (ImGui::Button(currentRenderMode.c_str())) {
        sys.window->SetSwapInterval(swapInterval == 0 ? 1 : 0);
    }

    ImGui::LabelText("FPS", std::to_string(Time::ComputeFPS()).c_str());

    ImGui::Text("Render Stats");

    ImGui::LabelText("Vertices", std::to_string(sys.renderStats.numVertices).c_str());
    ImGui::LabelText("Triangles", std::to_string(sys.renderStats.numTriangles).c_str());
    ImGui::LabelText("Meshes", std::to_string(sys.renderStats.numMeshes).c_str());
    ImGui::LabelText("Point lights", std::to_string(sys.renderStats.numPointLights).c_str());
    ImGui::LabelText("Directional lights", std::to_string(sys.renderStats.numDirectionalLights).c_str());

    return true;
}
} // namespace syst
} // namespace cfl
