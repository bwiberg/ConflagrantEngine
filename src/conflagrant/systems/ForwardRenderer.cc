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
#include <conflagrant/ShaderSourceManager.hh>

#include <imgui.h>

namespace cfl {
namespace syst {
ForwardRenderer::ForwardRenderer() {
    LoadShaders();
}

void ForwardRenderer::LoadShaders() {
    $
    forwardShader = LoadShader("forward.vert", "forward.frag");
    wireframeShader = LoadShader("wireframe.vert", "wireframe.frag");
    skydomeShader = LoadShader("forward_skydome.vert", "forward_skydome.frag");
    shadowmapLightpassShader = LoadShader("shadowmap_lightpass.vert", "shadowmap_lightpass.frag");
    shadowmapVisShader = LoadShader("shadowmap_visualization.vert", "shadowmap_visualization.frag");
}

void ForwardRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    $
    renderStats = RenderStats{};
    GLenum forwardShaderTextureCount = 0;

    {
        DOLLAR("Upload PointLight data")
        UploadPointLights<false>(entities, *forwardShader, renderStats);
    }

    {
        DOLLAR("Upload DirectionalLight data")
        UploadDirectionalLights<true>(entities, *forwardShader, *shadowmapLightpassShader,
                                      forwardShaderTextureCount, renderStats, cullModelsAndMeshes);
    }

    mat4 P;
    geometry::Frustum frustum;
    entityx::ComponentHandle<comp::Transform> cameraTransform;

    GetCameraInfo(entities, cameraTransform, frustum, P);
    mat4 V = glm::inverse(cameraTransform->GetMatrix());
    frustum = cameraTransform->GetMatrix() * frustum;

    uvec2 size = window->GetSize();
    OGL(glViewport(0, 0, size.x, size.y));
    OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

    {
        DOLLAR("Models")

        forwardShader->Bind();
        forwardShader->Uniform("V", V);
        forwardShader->Uniform("P", P);
        forwardShader->Uniform("EyePos", cameraTransform->Position());
        forwardShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_BACK));
        OGL(glEnable(GL_DEPTH_TEST));

        if (cullModelsAndMeshes) {
            RenderModels(entities, *forwardShader, forwardShaderTextureCount, renderStats, &frustum);
        } else {
            RenderModels(entities, *forwardShader, forwardShaderTextureCount, renderStats);
        }

        forwardShader->Unbind();
    }

    // Skydome
    {
        DOLLAR("Skydome")

        // only use rotational part for skydome
        auto const skydomeV = glm::inverse(glm::toMat4(cameraTransform->Quaternion()));

        skydomeShader->Bind();
        skydomeShader->Uniform("EyePos", cameraTransform->Position());
        skydomeShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_FRONT));
        OGL(glEnable(GL_DEPTH_TEST));

        RenderSkydomes(entities, *skydomeShader, 0, renderStats, P, skydomeV);

        skydomeShader->Unbind();
    }

    if (renderBoundingSpheres) {
        DOLLAR("Bounding spheres")

        wireframeShader->Bind();
        wireframeShader->Uniform("V", V);
        wireframeShader->Uniform("P", P);
        wireframeShader->Uniform("EyePos", cameraTransform->Position());
        wireframeShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

        OGL(glDisable(GL_CULL_FACE));
        OGL(glEnable(GL_DEPTH_TEST));

        RenderBoundingSpheres(entities, *wireframeShader, forwardShaderTextureCount, renderStats,
                              renderBoundingSpheresAsWireframe);
        wireframeShader->Unbind();
    }
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

    ImGui::Checkbox("Cull models and meshes", &sys.cullModelsAndMeshes);
    ImGui::Checkbox("Render bounding spheres", &sys.renderBoundingSpheres);
    if (sys.renderBoundingSpheres) {
        ImGui::Checkbox("- as wireframe", &sys.renderBoundingSpheresAsWireframe);
    }
    ImGui::LabelText("FPS", std::to_string(Time::ComputeFPS()).c_str());


    ImGui::Text("Render Stats");

    ImGui::LabelText("Vertices", std::to_string(sys.renderStats.numVertices).c_str());
    ImGui::LabelText("Triangles", std::to_string(sys.renderStats.numTriangles).c_str());

    ImGui::LabelText("Rendered models", std::to_string(sys.renderStats.numRenderedModels).c_str());
    ImGui::LabelText("Culled models", std::to_string(sys.renderStats.numCulledModels).c_str());
    ImGui::LabelText("Rendered meshes", std::to_string(sys.renderStats.numRenderedMeshes).c_str());
    ImGui::LabelText("Culled meshes", std::to_string(sys.renderStats.numCulledMeshes).c_str());

    ImGui::LabelText("Point lights", std::to_string(sys.renderStats.numPointLights).c_str());
    ImGui::LabelText("Directional lights", std::to_string(sys.renderStats.numDirectionalLights).c_str());

    return true;
}
} // namespace syst
} // namespace cfl
