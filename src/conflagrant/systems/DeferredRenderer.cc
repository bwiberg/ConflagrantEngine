#include "DeferredRenderer.hh"
#include "system_util.hh"
#include "ForwardRenderer.hh"

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
syst::DeferredRenderer::DeferredRenderer() {
    LoadShaders();
}

void syst::DeferredRenderer::LoadShaders() {
    geometryShader = LoadShader("deferred/geometry.vert", "deferred/geometry.frag");
    directionalLightShadowShader = LoadShader("shadowmap_lightpass.vert", "shadowmap_lightpass.frag");
    lightsShader = LoadShader("deferred/lights.vert", "deferred/lights.frag");
    skydomeShader = LoadShader("forward_skydome.vert", "forward_skydome.frag");
    wireframeShader = LoadShader("wireframe.vert", "wireframe.frag");
}

bool syst::DeferredRenderer::UpdateFramebuffer(GLsizei const width, GLsizei const height) {
    framebuffer = std::make_shared<gl::Framebuffer>(width, height);
    framebuffer->Bind();

    positionTexture = std::make_shared<gl::Texture2D>(width, height,
                                                      GL_RGB16F, GL_RGB, GL_FLOAT, nullptr);
    positionTexture->Bind();
    positionTexture->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    positionTexture->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    framebuffer->Attach(GL_COLOR_ATTACHMENT0, positionTexture);
    positionTexture->Unbind();

    normalShininessTexture = std::make_shared<gl::Texture2D>(width, height,
                                                             GL_RGBA16F, GL_RGBA, GL_FLOAT, nullptr);
    normalShininessTexture->Bind();
    normalShininessTexture->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    normalShininessTexture->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    framebuffer->Attach(GL_COLOR_ATTACHMENT1, normalShininessTexture);
    normalShininessTexture->Unbind();

    albedoSpecularTexture = std::make_shared<gl::Texture2D>(width, height,
                                                            GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    albedoSpecularTexture->Bind();
    albedoSpecularTexture->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    albedoSpecularTexture->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    framebuffer->Attach(GL_COLOR_ATTACHMENT2, albedoSpecularTexture);
    albedoSpecularTexture->Unbind();

    GLenum const attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    framebuffer->SetDrawBuffers(3, attachments);

    depthRenderbuffer = std::make_shared<gl::Renderbuffer>(width, height, GL_DEPTH_COMPONENT);
    framebuffer->Attach(GL_DEPTH_ATTACHMENT, depthRenderbuffer);

    if (!framebuffer->CheckIsComplete()) {
        LOG_ERROR(cfl::DeferredRenderer::UpdateFramebuffer) << "Framebuffer incomplete";
        return false;
    }

    return true;
}

void
syst::DeferredRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    auto const& factories = engine->orderedSystemFactories;
    auto itForward = std::find_if(factories.begin(), factories.end(), [](std::shared_ptr<SystemFactory> const factory) {
        return factory->GetName() == "ForwardRenderer";
    });
    auto itDeferred = std::find_if(factories.begin(), factories.end(), [](std::shared_ptr<SystemFactory> const factory) {
        return factory->GetName() == "DeferredRenderer";
    });
    if (itForward < itDeferred) return;

    uvec2 size = window->GetSize();
    auto const width = static_cast<GLsizei>(size.x);
    auto const height = static_cast<GLsizei>(size.y);

    if (size != lastWindowSize) {
        lastWindowSize = size;
        if (!UpdateFramebuffer(width, height)) {
            return;
        }
    }

    // framebuffer is ready to go

    renderStats.Reset();

    mat4 P;
    geometry::Frustum frustum;
    entityx::ComponentHandle<comp::Transform> cameraTransform;

    GetCameraInfo(entities, cameraTransform, frustum, P);
    mat4 V = glm::inverse(cameraTransform->GetMatrix());
    frustum = cameraTransform->GetMatrix() * frustum;

    {
        DOLLAR("Deferred: Geometry pass")
        framebuffer->Bind();
        OGL(glViewport(0, 0, width, height));
        OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        geometryShader->Bind();
        geometryShader->Uniform("V", V);
        geometryShader->Uniform("P", P);
        geometryShader->Uniform("EyePos", cameraTransform->Position());
        geometryShader->Uniform("time", static_cast<float>(Time::CurrentTime()));
        renderStats.UniformCalls += 4;

        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_BACK));
        OGL(glEnable(GL_DEPTH_TEST));

        if (cullModelsAndMeshes) {
            RenderModels(entities, *geometryShader, 0, renderStats, &frustum);
        } else {
            RenderModels(entities, *geometryShader, 0, renderStats);
        }

        geometryShader->Unbind();
    }

    GLenum lightsShaderTextureCount = 0;

    {
        DOLLAR("Deferred: Upload PointLights")
        UploadPointLights<false>(entities, *lightsShader, renderStats);
    }

    {
        DOLLAR("Upload DirectionalLight data")
        UploadDirectionalLights<true>(entities, *lightsShader, *directionalLightShadowShader,
                                      lightsShaderTextureCount, renderStats, cullModelsAndMeshes);
    }

    {
        DOLLAR("Deferred: All lights pass")

        OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        OGL(glViewport(0, 0, width, height));
        OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        lightsShader->Bind();

        lightsShader->Uniform("V", V);
        lightsShader->Uniform("P", P);
        lightsShader->Uniform("EyePos", cameraTransform->Position());
        lightsShader->Uniform("time", static_cast<float>(Time::CurrentTime()));
        lightsShader->Texture("GPosition", lightsShaderTextureCount++, *positionTexture);
        lightsShader->Texture("GNormalShininess", lightsShaderTextureCount++, *normalShininessTexture);
        lightsShader->Texture("GAlbedoSpecular", lightsShaderTextureCount++, *albedoSpecularTexture);
        renderStats.UniformCalls += 7;

        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_BACK));
        OGL(glEnable(GL_DEPTH_TEST));

        RenderFullscreenQuad(renderStats);

        lightsShader->Unbind();
    }

    {
        DOLLAR("Deferred: Blit framebuffer depth")

        framebuffer->Bind(GL_READ_FRAMEBUFFER);

        OGL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        OGL(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST));
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        framebuffer->Unbind();
    }

    {
        DOLLAR("Deferred: Render skydome")

        // only use rotational part for skydome
        auto const skydomeV = glm::inverse(glm::toMat4(cameraTransform->Quaternion()));

        skydomeShader->Bind();

        skydomeShader->Uniform("EyePos", cameraTransform->Position());
        skydomeShader->Uniform("time", static_cast<float>(Time::CurrentTime()));
        renderStats.UniformCalls += 2;

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
        renderStats.UniformCalls += 4;

        OGL(glDisable(GL_CULL_FACE));
        OGL(glEnable(GL_DEPTH_TEST));

        RenderBoundingSpheres(entities, *wireframeShader, 0, renderStats,
                              renderBoundingSpheresAsWireframe);
        wireframeShader->Unbind();
    }
}

bool syst::DeferredRenderer::DrawWithImGui(syst::DeferredRenderer &sys, InputManager const &input) {
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
    ImGui::LabelText("ms/frame", std::to_string(Time::ComputeAverageFrametime()).c_str());

    ImGui::Text("Render Stats");
    sys.renderStats.DrawWithImGui();

    return true;
}
} // namespace cfl
