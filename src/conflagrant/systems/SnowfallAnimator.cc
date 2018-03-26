#include "SnowfallAnimator.hh"
#include "DeferredRenderer.hh"

#include <conflagrant/components/SnowEmitter.hh>
#include <conflagrant/systems/DeferredRenderer.hh>
#include <conflagrant/ShaderSourceManager.hh>
#include <conflagrant/Time.hh>
#include <conflagrant/systems/system_util.hh>
#include <conflagrant/gl/State.hh>
#include <conflagrant/Engine.hh>

namespace cfl {
namespace syst {
void Pingpong(comp::SnowEmitter &snow) {
    snow.framebuffer->Bind();

    snow.positionsAngles->Swap();
    snow.framebuffer->Attach(GL_COLOR_ATTACHMENT0, snow.positionsAngles->Front());

    snow.velocitiesLifetimes->Swap();
    snow.framebuffer->Attach(GL_COLOR_ATTACHMENT1, snow.velocitiesLifetimes->Front());

    GLenum const attachments[2] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
    };
    snow.framebuffer->SetDrawBuffers(2, attachments);

    snow.framebuffer->Unbind();
}

SnowfallAnimator::SnowfallAnimator() {
    LoadShaders();
}

void SnowfallAnimator::LoadShaders() {
    simulateComputeShader = LoadComputeShader("snow/snowfall_simulate.comp");
    sortByDepthComputeShader = LoadComputeShader("snow/snowfall_depthsort.comp");
}

void SnowfallAnimator::ResetTopDownFramebuffer(GLsizei size) {
    heightFramebuffer = std::make_shared<gl::Framebuffer>(size, size);
    heightFramebuffer->Bind();

    heightTexture = std::make_shared<gl::Texture2D>(size, size, GL_R32F, GL_RED, GL_FLOAT, nullptr);
    heightTexture->Bind();
    heightTexture->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    heightTexture->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    heightTexture->Unbind();
    heightFramebuffer->Attach(GL_COLOR_ATTACHMENT0, heightTexture);

    GLenum const attachments[1] = {
            GL_COLOR_ATTACHMENT0
    };
    heightFramebuffer->SetDrawBuffers(1, attachments);

    heightFramebuffer->Attach(GL_DEPTH_ATTACHMENT,
                              std::make_shared<gl::Renderbuffer>(size, size, GL_DEPTH_COMPONENT));

    if (!heightFramebuffer->CheckIsComplete()) {
        LOG_ERROR(cfl::DeferredRenderer::UpdateFramebuffer) << "Framebuffer incomplete";
    }

    heightFramebuffer->Unbind();
}

void SnowfallAnimator::RenderTopDownDepthTexture() {

}

void SnowfallAnimator::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    if (!enabled)
        return;

    auto const heightTextureSize = static_cast<GLsizei>(math::Pow(2, heightTexturePower));
    if (!heightFramebuffer || heightFramebuffer->width != heightTextureSize) {
        ResetTopDownFramebuffer(heightTextureSize);
        RenderTopDownDepthTexture();
    }

    // --------------- //
    // common uniforms //
    // --------------- //

    auto const& deferred = engine->GetSystemManager()->system<DeferredRenderer>();

    auto const& sceneVoxelTexture = deferred->voxelTexture;
    auto const& voxelHalfDimensions = deferred->VCT.halfDimensions;
    auto const& voxelCenter = deferred->VCT.center;

    auto const time = static_cast<float>(Time::CurrentTime());
    auto const timeDelta = static_cast<float>(Time::DeltaTime());

    uvec2 size = window->GetSize();
    auto const width = static_cast<GLsizei>(size.x);
    auto const height = static_cast<GLsizei>(size.y);

    mat4 P;
    geometry::Frustum frustum;
    entityx::ComponentHandle<comp::Transform> cameraTransform;
    float zNear, zFar;

    GetCameraInfo(entities, cameraTransform, frustum, P, zNear, zFar);
    auto const V = glm::inverse(cameraTransform->GetMatrix());
    auto const EyePos = cameraTransform->Position();
    frustum = cameraTransform->GetMatrix() * frustum;

    renderStats.Reset();

    {
        simulateComputeShader->Bind();
        simulateComputeShader->Uniform("time", time);
        simulateComputeShader->Uniform("timeDelta", timeDelta);

        simulateComputeShader->Uniform("VoxelHalfDimensions", vec3(voxelHalfDimensions));
        simulateComputeShader->Uniform("VoxelCenter", voxelCenter);
        simulateComputeShader->Texture("VoxelizedScene", 2, *sceneVoxelTexture);

        entityx::ComponentHandle<comp::SnowEmitter> snow;
        for (auto e : entities.entities_with_components(snow)) {
            if (snow->count == 0) {
                continue;
            }

            auto const oldMaxCount = snow->maxCount;
            snow->maxCount = math::NextPowerOfTwo(snow->count);
            if (!snow->framebuffer || oldMaxCount != snow->maxCount) {
                InitializeComponent(*snow);
            }

            mat4 snowEmitterTransform(1);
            auto const transform = e.component<comp::Transform>();
            if (transform) {
                snowEmitterTransform = transform->GetMatrix();
            }

            simulateComputeShader->Uniform("Count", static_cast<GLint>(snow->count));
            simulateComputeShader->Uniform("MaxCount", static_cast<GLint>(snow->maxCount));
            simulateComputeShader->Uniform("EmitterTransform", snowEmitterTransform * glm::scale(snow->dimensions));

            simulateComputeShader->Texture("PositionsAngles", 0, *snow->positionsAngles->Front());
            simulateComputeShader->Texture("VelocitiesLifetimes", 1, *snow->velocitiesLifetimes->Front());
            OGL(glBindImageTexture(0, snow->positionsAngles->Front()->ID(),
                                   0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
            OGL(glBindImageTexture(1, snow->velocitiesLifetimes->Front()->ID(),
                                   0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));

            auto computeSize = static_cast<GLuint>(snow->count);
            OGL(glDispatchCompute(computeSize, 1, 1));
        }

        simulateComputeShader->Unbind();
    }

    if (sortByDepth) {
        sortByDepthComputeShader->Bind();
        sortByDepthComputeShader->Uniform("time", time);
        sortByDepthComputeShader->Uniform("timeDelta", timeDelta);
        sortByDepthComputeShader->Uniform("EyePos", EyePos);

        entityx::ComponentHandle<comp::SnowEmitter> snow;
        for (auto e : entities.entities_with_components(snow)) {
            if (snow->count == 0) {
                continue;
            }

            auto const oldMaxCount = snow->maxCount;
            snow->maxCount = math::NextPowerOfTwo(snow->count);
            if (!snow->framebuffer || oldMaxCount != snow->maxCount) {
                InitializeComponent(*snow);
            }

            sortByDepthComputeShader->Uniform("Count", static_cast<GLint>(snow->count));
            sortByDepthComputeShader->Uniform("MaxCount", static_cast<GLint>(snow->maxCount));

            sortByDepthComputeShader->Texture("PositionsAngles", 0, *snow->positionsAngles->Front());
            sortByDepthComputeShader->Texture("VelocitiesLifetimes", 1, *snow->velocitiesLifetimes->Front());

            OGL(glBindImageTexture(0, snow->positionsAngles->Front()->ID(),
                                   0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
            OGL(glBindImageTexture(1, snow->velocitiesLifetimes->Front()->ID(),
                                   0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));

            auto const computeSize = static_cast<GLuint>(math::NextEvenNumber(snow->count) / 2);

            for (int i = 0; i < 2; ++i) {
                sortByDepthComputeShader->Uniform("GlobalOffset", i);
                OGL(glDispatchCompute(computeSize, 1, 1));
            }
        }

        sortByDepthComputeShader->Unbind();
    }
}

bool SnowfallAnimator::Serialize(BaseSerializer const &serializer, Json::Value &json, SnowfallAnimator &sys) {
    json["name"] = SystemName;
    SERIALIZE(cfl::SnowfallAnimator, json["heightTexturePower"], sys.heightTexturePower);
    return true;
}

bool SnowfallAnimator::DrawWithImGui(SnowfallAnimator &sys, InputManager const &input) {
    ImGui::Checkbox("Enabled", &sys.enabled);

    if (ImGui::Button("Reload shaders")) {
        sys.LoadShaders();
    }

    ImGui::Checkbox("Sort by depth", &sys.sortByDepth);

    return true;
}
} // namespace syst
} // namespace cfl
