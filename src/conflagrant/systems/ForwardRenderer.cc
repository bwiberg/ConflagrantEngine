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
    GLenum forwardShaderTextureCount = 0;

    {
        DOLLAR("Upload PointLight data")
        UploadPointLights<false>(entities, *forwardShader, renderStats);
    }

    {
        DOLLAR("Upload DirectionalLight data")
        UploadDirectionalLights<true>(entities, *forwardShader, *shadowmapLightpassShader,
                                      forwardShaderTextureCount, renderStats);
    }

    mat4 P;
    geometry::Frustum frustum;
    entityx::ComponentHandle<comp::Transform> cameraTransform;

    GetCameraInfo(entities, cameraTransform, frustum, P);
    mat4 V = glm::inverse(cameraTransform->GetMatrix());
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

        RenderModels(entities, *forwardShader, forwardShaderTextureCount, renderStats);

        forwardShader->Unbind();
    }

    // Skydome
    {
        DOLLAR("Skydome")

        // only use rotational part for skydome
        V = glm::inverse(glm::toMat4(cameraTransform->Quaternion()));

        skydomeShader->Bind();
        forwardShader->Uniform("EyePos", cameraTransform->Position());
        forwardShader->Uniform("time", static_cast<float>(Time::CurrentTime()));

        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_FRONT));
        OGL(glEnable(GL_DEPTH_TEST));

        RenderSkydomes(entities, *skydomeShader, 0, renderStats, P, V);

        skydomeShader->Unbind();
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
