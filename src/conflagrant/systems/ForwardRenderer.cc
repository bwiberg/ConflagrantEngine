#include "ForwardRenderer.hh"

#include <conflagrant/systems/CameraController.hh>
#include <conflagrant/Engine.hh>
#include <conflagrant/Time.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/PointLight.hh>
#include <conflagrant/components/DirectionalLight.hh>

#include <imgui.h>
#include <fstream>

namespace cfl {
namespace syst {
ForwardRenderer::ForwardRenderer() {
    LoadShaders();
}

void ForwardRenderer::LoadShaders() {
    Path forwardVertexPath("forward.vert"), forwardFragmentPath("forward.frag");
    {
        PathResolver resolver;
        resolver.append(Path(BUILTIN_SHADER_DIR));

        forwardVertexPath = resolver.resolve(forwardVertexPath);
        forwardFragmentPath = resolver.resolve(forwardFragmentPath);
    }

    if (!forwardVertexPath.is_file() || !forwardFragmentPath.is_file()) {
        LOG_ERROR(cfl::syst::ForwardRenderer)
                << "Fatal error, couldn't locate built-in shader sources for 'forward' shader" << std::endl;
        return;
    }

    std::stringstream buffer;

    buffer << std::ifstream(forwardVertexPath.str()).rdbuf();
    string const forwardShaderVertex = buffer.str();
    buffer.str("");

    buffer << std::ifstream(forwardFragmentPath.str()).rdbuf();
    string const forwardShaderFragment = buffer.str();
    buffer.str("");

    forwardShader = std::make_shared<gl::Shader>(forwardShaderVertex, forwardShaderFragment);
}

void SetCameraUniforms(entityx::EntityManager &entities, gl::Shader &shader) {
    using entityx::ComponentHandle;

    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::ActiveCamera> active;
    ComponentHandle<comp::PerspectiveCamera> perspective;
    ComponentHandle<comp::OrthographicCamera> orthographic;

    entityx::Entity activeCamera;
    for (auto entity : entities.entities_with_components(transform, active, perspective)) {
        activeCamera = entity;
        break;
    }

    if (!activeCamera.valid()) {
        for (auto entity : entities.entities_with_components(transform, active, orthographic)) {
            activeCamera = entity;
            break;
        }
    }

    mat4 V;
    if (transform) {
        V = glm::inverse(transform->GetMatrix());
    } else {
        V = mat4(1);
    }
    shader.Uniform("V", V);

    mat4 P;
    if (perspective) {
        P = perspective->projection;
    } else if (orthographic) {
        P = orthographic->projection;
    } else {
        LOG_ERROR(cfl::ForwardRenderer::SetCameraMatrices)
                << "No OrthographicCamera or PerspectiveCamera." << std::endl;
        P = mat4(1);
    }
    shader.Uniform("P", P);

    shader.Uniform("EyePos", transform->Position());
}

void ForwardRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    renderStats = RenderStats{};

    uvec2 size = window->GetSize();
    OGL(glViewport(0, 0, size.x, size.y));
    OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));


    using entityx::ComponentHandle;
    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::Model> model;
    ComponentHandle<comp::PointLight> pointLight;
    ComponentHandle<comp::DirectionalLight> directionalLight;

    forwardShader->Bind();

    forwardShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

    // upload camera parameters
    SetCameraUniforms(entities, *forwardShader);

    // upload PointLight data
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
    renderStats.numPointLights = ilight;

    // upload DirectionalLight data
    ilight = 0;
    ss.str("");
    for (auto const &entity : entities.entities_with_components(directionalLight)) {
        ss << "directionalLights" << "[" << ilight << "]" << ".";
        string const prefix = ss.str();

        float const phi = glm::radians(directionalLight->horizontal);
        float const theta = glm::radians(90 - directionalLight->vertical);
        vec3 direction(sinf(theta) * cosf(phi), cosf(theta), sinf(theta) * sinf(phi));

        forwardShader->Uniform(prefix + "direction", direction);
        forwardShader->Uniform(prefix + "intensity", directionalLight->intensity);
        forwardShader->Uniform(prefix + "color", directionalLight->color);

        ilight++;
        ss.str("");
    }
    forwardShader->Uniform("numDirectionalLights", ilight);
    renderStats.numDirectionalLights = ilight;

    OGL(glEnable(GL_CULL_FACE));
    OGL(glCullFace(GL_BACK));
    OGL(glEnable(GL_DEPTH_TEST));

    for (auto const &entity : entities.entities_with_components(transform, model)) {
        forwardShader->Uniform("M", transform->GetMatrix());

        for (auto const &part : model->value->parts) {
            string const prefix = "material.";

            // upload material
            {
                auto const &material = *part.second;

                {
                    string const diffusePrefix = prefix + "diffuse.";
                    forwardShader->Uniform(diffusePrefix + "color", material.diffuseColor);

                    int hasDiffuseMap = (material.diffuseTexture != nullptr) ? 1 : 0;
                    forwardShader->Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
                    if (hasDiffuseMap == 1) {
                        forwardShader->Texture(diffusePrefix + "map", 0, material.diffuseTexture->texture);
                    }
                }

                {
                    string const specularPrefix = prefix + "specular.";
                    forwardShader->Uniform(specularPrefix + "color", material.specularColor);

                    int hasSpecularMap = (material.specularTexture != nullptr) ? 1 : 0;
                    forwardShader->Uniform(specularPrefix + "hasMap", hasSpecularMap);
                    if (hasSpecularMap == 1) {
                        forwardShader->Texture(specularPrefix + "map", 1, material.specularTexture->texture);
                    }
                }

                // todo implement normal mapping
                int hasNormalMap = 0;
                forwardShader->Uniform(prefix + "hasNormalMap", hasNormalMap);
                if (hasNormalMap == 1) {
                    // forwardShader->Texture(prefix + "normalMap", GL_TEXTURE2, material.ambientTexture->texture);
                    assert(false && "Unreachable code!");
                }

                forwardShader->Uniform(prefix + "shininess", material.shininess);
            }

            // render mesh
            {
                auto const &mesh = *part.first;
                mesh.glMesh->DrawElements();

                renderStats.numMeshes++;
                renderStats.numTriangles += mesh.triangles.size();
                renderStats.numVertices += mesh.vertices.size();
            }

        }
    }

    forwardShader->Unbind();
}

bool ForwardRenderer::DrawWithImGui(ForwardRenderer &sys, InputManager const &input) {
    if (ImGui::Button("Reload shaders")) {
        sys.LoadShaders();
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
