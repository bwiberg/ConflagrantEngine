#include "DeferredRenderer.hh"
#include "system_util.hh"
#include "ForwardRenderer.hh"

#include <conflagrant/systems/CameraController.hh>
#include <conflagrant/Engine.hh>
#include <conflagrant/Time.hh>
#include <conflagrant/Timer.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/PointLight.hh>
#include <conflagrant/components/DirectionalLight.hh>
#include <conflagrant/components/DirectionalLightShadow.hh>
#include <conflagrant/components/Skydome.hh>
#include <conflagrant/components/SnowEmitter.hh>
#include <conflagrant/ShaderSourceManager.hh>

#include <imgui.h>

#define TIMER(name) cfl::Timer<double> __timer__ ## name (durationsByName[(#name)])

namespace cfl {
syst::DeferredRenderer::DeferredRenderer() {
    timeSnowStart = -1;

    LoadShaders();

    pointMesh = std::make_shared<gl::Mesh>();

    pointMesh->Attribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

    vec3 const zero(0, 0, 0);
    pointMesh->BufferVertexData(sizeof(vec3), glm::value_ptr(zero), GL_STATIC_DRAW);
    pointMesh->SetDrawMode(GL_POINTS);
}

void syst::DeferredRenderer::LoadShaders() {
    geometryShader = LoadShader("deferred/geometry.vert", "deferred/geometry.frag");
    snowGeometryShader = LoadShader("snow/snowgeometry.vert", "snow/snowgeometry.geom", "snow/snowgeometry.frag");
    directionalLightShadowShader = LoadShader("shadowmap_lightpass.vert", "shadowmap_lightpass.frag");
    lightsShader = LoadShader("deferred/lights.vert", "deferred/lights.frag");
    skydomeShader = LoadShader("forward_skydome.vert", "forward_skydome.frag");
    wireframeShader = LoadShader("wireframe.vert", "wireframe.frag");
    snowfallParticleShader = LoadShader("snow/snowfall_render.vert", "snow/snowfall_render.geom", "snow/snowfall_render.frag");

#ifdef ENABLE_VOXEL_CONE_TRACING
    voxelizeShader = LoadShader("voxels/voxelize.vert", "voxels/voxelize.geom", "voxels/voxelize.frag");
    voxelDirectRenderingShader = LoadShader("voxels/directrendering.vert", "voxels/directrendering.frag");
    voxelConeTracingShader = LoadShader("voxels/conetracing.vert", "voxels/conetracing.frag");
    mipmapShader = LoadComputeShader("voxels/mipmap.comp");
#endif
}

bool syst::DeferredRenderer::UpdateFramebuffer(GLsizei const width, GLsizei const height) {
    framebuffer = std::make_shared<gl::Framebuffer>(width, height);
    framebuffer->Bind();

    positionRadianceTexture = std::make_shared<gl::DoubleBufferedTexture2D>(width, height,
                                                              GL_RGBA16F, GL_RGBA, GL_FLOAT);
    positionRadianceTexture->ApplyToBoth([](auto &tex) {

        tex->Bind();
        tex->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->Unbind();
    });
    framebuffer->Attach(GL_COLOR_ATTACHMENT0, positionRadianceTexture->Front());

    normalShininessTexture = std::make_shared<gl::DoubleBufferedTexture2D>(width, height,
                                                                       GL_RGBA16F, GL_RGBA, GL_FLOAT);
    normalShininessTexture->ApplyToBoth([](auto &tex) {

        tex->Bind();
        tex->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->Unbind();
    });
    framebuffer->Attach(GL_COLOR_ATTACHMENT1, normalShininessTexture->Front());

    albedoSpecularTexture = std::make_shared<gl::DoubleBufferedTexture2D>(width, height,
                                                                      GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    albedoSpecularTexture->ApplyToBoth([](auto &tex) {

        tex->Bind();
        tex->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->Unbind();
    });
    framebuffer->Attach(GL_COLOR_ATTACHMENT2, albedoSpecularTexture->Front());

    depthTexture = std::make_shared<gl::DoubleBufferedTexture2D>(width, height, GL_R32F, GL_RED, GL_FLOAT);
    depthTexture->ApplyToBoth([](auto &tex) {
        tex->Bind();
        tex->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->Unbind();
    });
    framebuffer->Attach(GL_COLOR_ATTACHMENT3, depthTexture->Front());

    GLenum const attachments[4] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1,
            GL_COLOR_ATTACHMENT2,
            GL_COLOR_ATTACHMENT3
    };
    framebuffer->SetDrawBuffers(4, attachments);

    depthRenderbuffer = std::make_shared<gl::Renderbuffer>(width, height, GL_DEPTH_COMPONENT);
    framebuffer->Attach(GL_DEPTH_ATTACHMENT, depthRenderbuffer);

    if (!framebuffer->CheckIsComplete()) {
        LOG_ERROR(cfl::DeferredRenderer::UpdateFramebuffer) << "Framebuffer incomplete";
        return false;
    }

    return true;
}

void syst::DeferredRenderer::Pingpong() {
    framebuffer->Bind();

    positionRadianceTexture->Swap();
    framebuffer->Attach(GL_COLOR_ATTACHMENT0, positionRadianceTexture->Front());

    normalShininessTexture->Swap();
    framebuffer->Attach(GL_COLOR_ATTACHMENT1, normalShininessTexture->Front());

    albedoSpecularTexture->Swap();
    framebuffer->Attach(GL_COLOR_ATTACHMENT2, albedoSpecularTexture->Front());

    depthTexture->Swap();
    framebuffer->Attach(GL_COLOR_ATTACHMENT3, depthTexture->Front());

    framebuffer->Unbind();
}

void
syst::DeferredRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    for (auto &kvp : durationsByName) {
        kvp.second = 0.0;
    }

    auto const& factories = engine->orderedSystemFactories;
    auto itForward = std::find_if(factories.begin(), factories.end(), [](std::shared_ptr<SystemFactory> const factory) {
        return factory->GetName() == "ForwardRenderer";
    });
    auto itDeferred = std::find_if(factories.begin(), factories.end(), [](std::shared_ptr<SystemFactory> const factory) {
        return factory->GetName() == "DeferredRenderer";
    });
    if (itForward < itDeferred) return;

    /////////////
    // hotkeys //
    /////////////

    timeSnowStart = (timeSnowStart == -1) ? Time::CurrentTime() : timeSnowStart;
    if (input->GetKeyDown(Key::DIGIT_9)) {
        isSnowing = !isSnowing;
        timeSnowStart = Time::CurrentTime();
    }

#define TOGGLE(boolean) (boolean) = !(boolean)
#define TOGGLE_ON_KEY(key, boolean) if (input->GetKeyDown(key)) { TOGGLE(boolean); }

    TOGGLE_ON_KEY(Key::V,       useVoxelConeTracing);
    TOGGLE_ON_KEY(Key::DIGIT_0, VCT.useDirectVoxelRendering);
    TOGGLE_ON_KEY(Key::DIGIT_1, VCT.useDirectLighting);
    TOGGLE_ON_KEY(Key::DIGIT_2, VCT.useIndirectDiffuseLighting);
    TOGGLE_ON_KEY(Key::DIGIT_3, VCT.useIndirectSpecularLighting);

#undef TOGGLE_ON_KEY
#undef TOGGLE

    uvec2 size = window->GetSize();
    auto const width = static_cast<GLsizei>(size.x);
    auto const height = static_cast<GLsizei>(size.y);
    vec2 const ScreenSize(width, height);

    if (size != lastWindowSize) {
        DOLLAR("Deferred: Update framebuffer")
        lastWindowSize = size;
        if (!UpdateFramebuffer(width, height)) {
            return;
        }
    }

    Pingpong();
    // framebuffer is ready to go

    renderStats.Reset();

    auto const timeCurrent = static_cast<float>(Time::CurrentTime());
    auto const timeDelta = static_cast<float>(Time::DeltaTime());

    mat4 P;
    geometry::Frustum frustum;
    entityx::ComponentHandle<comp::Transform> cameraTransform;
    float zNear, zFar;

    GetCameraInfo(entities, cameraTransform, frustum, P, zNear, zFar);
    mat4 const V = glm::inverse(cameraTransform->GetMatrix());
    frustum = cameraTransform->GetMatrix() * frustum;

    auto const EyePos = cameraTransform->Position();

    {
        auto shader = (isSnowing) ? snowGeometryShader : geometryShader;

        TIMER(GeometryPass);
        DOLLAR("Deferred: Geometry pass")
        framebuffer->Bind();
        OGL(glViewport(0, 0, width, height));
        OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
        OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        shader->Bind();
        shader->Uniform("V", V);
        shader->Uniform("P", P);
        shader->Uniform("EyePos", EyePos);
        shader->Uniform("time", timeCurrent);
        shader->Uniform("timeSnowStart", static_cast<float>(timeSnowStart));
        shader->Uniform("ScreenSize", ScreenSize);
        renderStats.UniformCalls += 4;

        auto scopedState = gl::ScopedState()
                .Enable(GL_CULL_FACE)
                .CullFace(GL_BACK)
                .Enable(GL_DEPTH_TEST)
                .Build();

        OGL(glEnable(GL_CULL_FACE));
        OGL(glCullFace(GL_BACK));
        OGL(glEnable(GL_DEPTH_TEST));

        if (cullModelsAndMeshes) {
            RenderModels(entities, *shader, 0, renderStats, &frustum);
        } else {
            RenderModels(entities, *shader, 0, renderStats);
        }

        shader->Unbind();
    }

    GLenum lightsShaderTextureCount = 0;

    {
        TIMER(DeferredUploadLights);
        {
            DOLLAR("Deferred: Upload PointLights")
            UploadPointLights<false>(entities, *lightsShader, renderStats);
        }
        {
            DOLLAR("Upload DirectionalLight data")

            auto scopedState = gl::ScopedState()
                    .Enable(GL_CULL_FACE)
                    .CullFace(GL_BACK)
                    .Enable(GL_DEPTH_TEST)
                    .Build();

            RenderDirectionalLightShadows(entities, *directionalLightShadowShader, renderStats, cullModelsAndMeshes);
            UploadDirectionalLights<true>(entities, *lightsShader, lightsShaderTextureCount, renderStats, cullModelsAndMeshes);
        }
    }

#ifdef ENABLE_VOXEL_CONE_TRACING
    if (useVoxelConeTracing) {
        auto const voxelTextureSize = GetActualVoxelTextureSize();
        GLenum voxelizeShaderTextureCount = 0, voxelConeTracingShaderTextureCount = 0;

        {
            TIMER(VctAllocateClearTextures);
            DOLLAR("Deferred: Prepare for VCT")

            if (!voxelTexture ||
                voxelTexture->width != voxelTextureSize  ||
                voxelTexture->height != voxelTextureSize ||
                voxelTexture->depth != voxelTextureSize  ||
                voxelTexture->mipmapLevels != VCT.mipmapLevels) {

                voxelTexture = std::make_shared<gl::Texture3D>(voxelTextureSize, voxelTextureSize, voxelTextureSize,
                                                               GL_RGBA8, GL_RGBA, GL_FLOAT,
                                                               nullptr, VCT.mipmapLevels);

                voxelTexture->Bind();

                voxelTexture->TexParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                voxelTexture->TexParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                voxelTexture->TexParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

                voxelTexture->TexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                voxelTexture->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                voxelTexture->Unbind();
            }

            voxelTexture->ClearTexImage();
        }

        {
            TIMER(VctUploadLights);
            DOLLAR("Deferred (VCT): Upload lights")
            UploadPointLights<false>(entities, *voxelizeShader, renderStats);
            UploadDirectionalLights<true>(entities, *voxelizeShader, voxelizeShaderTextureCount,
                                          renderStats, cullModelsAndMeshes);
        }

        {
            TIMER(VctVoxelizeScene);
            DOLLAR("Deferred (VCT): Voxelize scene")

            geometry::Frustum const voxelFrustum{
                    .sides = {
                            geometry::Plane{
                                    .center = VCT.center + VCT.halfDimensions * geometry::Backward,
                                    .normal = geometry::Backward
                            },
                            geometry::Plane{
                                    .center = VCT.center + VCT.halfDimensions * geometry::Forward,
                                    .normal = geometry::Forward
                            },
                            geometry::Plane{
                                    .center = VCT.center + VCT.halfDimensions * geometry::Left,
                                    .normal = geometry::Left
                            },
                            geometry::Plane{
                                    .center = VCT.center + VCT.halfDimensions * geometry::Right,
                                    .normal = geometry::Right
                            },
                            geometry::Plane{
                                    .center = VCT.center + VCT.halfDimensions * geometry::Down,
                                    .normal = geometry::Down
                            },
                            geometry::Plane{
                                    .center = VCT.center + VCT.halfDimensions * geometry::Up,
                                    .normal = geometry::Up
                            }
                    }
            };

            OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            OGL(glViewport(0, 0, voxelTextureSize, voxelTextureSize));

            auto scopedState = gl::ScopedState()
                    .ColorMask(GL_FALSE)
                    .Disable(GL_CULL_FACE)
                    .Disable(GL_DEPTH_TEST)
                    .Disable(GL_BLEND)
                    .Build();

            voxelizeShader->Bind();

            voxelizeShader->Uniform("V", geometry::Identity4);
            voxelizeShader->Uniform("P", geometry::Identity4);

            voxelizeShader->Uniform("VoxelHalfDimensions", vec3(VCT.halfDimensions));
            voxelizeShader->Uniform("VoxelCenter", VCT.center);

            auto const voxelizedSceneTextureUnit = voxelizeShaderTextureCount++;
            voxelizeShader->Texture("VoxelizedScene", voxelizedSceneTextureUnit, *voxelTexture);
            OGL(glBindImageTexture(voxelizedSceneTextureUnit, voxelTexture->ID(),
                                   0, GL_TRUE, 0, GL_READ_WRITE, GL_R32UI));

            renderStats.UniformCalls += 5;

            if (cullModelsAndMeshes) {
                RenderModels(entities, *voxelizeShader, voxelizeShaderTextureCount, renderStats, &voxelFrustum);
            } else {
                RenderModels(entities, *voxelizeShader, voxelizeShaderTextureCount, renderStats);
            }

            voxelizeShader->Unbind();
        }

        if (Time::CurrentTime() - VCT.timeOfLastMipmapGeneration >= VCT.timeBetweenMipmapGeneration) {
            TIMER(VctGenerateMipmap);
            DOLLAR("Deferred (VCT): Generate voxel mipmap")
            VCT.timeOfLastMipmapGeneration = Time::CurrentTime();

            if (VCT.useComputeShaderMipmapper) {
                mipmapShader->Bind();

                auto computeSize = static_cast<GLuint>(GetActualVoxelTextureSize() / 2);
                for (decltype(VCT.mipmapLevels) level = 0; level < VCT.mipmapLevels; ++level) {
                    mipmapShader->Uniform("IsFirstLevel", 1 - math::Clamp(level, 0, 1));

                    mipmapShader->Texture("ImageSource", 0, *voxelTexture);
                    OGL(glBindImageTexture(0, voxelTexture->ID(), level, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA8));
                    mipmapShader->Texture("ImageMipmap", 1, *voxelTexture);
                    OGL(glBindImageTexture(1, voxelTexture->ID(), level + 1, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8));

                    OGL(glDispatchCompute(computeSize, computeSize, computeSize));

                    computeSize /= 2;
                }

                mipmapShader->Unbind();
            } else {
                voxelTexture->GenerateMipmap();
            }
        }

        if (VCT.useDirectVoxelRendering) {
            TIMER(VctDirectRendering);
            DOLLAR("Deferred (VCT): Direct voxel rendering")

            OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            OGL(glViewport(0, 0, width, height));
            OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            voxelDirectRenderingShader->Bind();

            voxelDirectRenderingShader->Uniform("InverseVP", glm::inverse(P * V));
            voxelDirectRenderingShader->Uniform("EyePos", EyePos);

            voxelDirectRenderingShader->Uniform("RenderDistance", VCT.DirectRendering.renderDistance);
            voxelDirectRenderingShader->Texture("VoxelizedScene", 0, *voxelTexture);
            voxelDirectRenderingShader->Uniform("VoxelHalfDimensions", vec3(VCT.halfDimensions));
            voxelDirectRenderingShader->Uniform("VoxelCenter", VCT.center);
            voxelDirectRenderingShader->Uniform("MipmapLevel", VCT.DirectRendering.mipmapLevel);
            voxelDirectRenderingShader->Uniform("NumSteps", VCT.DirectRendering.raymarchingSteps);

            renderStats.UniformCalls += 8;

            auto scopedState = gl::ScopedState()
                    .Enable(GL_CULL_FACE)
                    .CullFace(GL_BACK)
                    .Enable(GL_DEPTH_TEST)
                    .Build();

            RenderFullscreenQuad(renderStats);

            voxelDirectRenderingShader->Unbind();

            return;
        }

        else {
            {
                TIMER(VctUploadLights);
                DOLLAR("Deferred (VCT): Upload lights")
                UploadPointLights<false>(entities, *voxelConeTracingShader, renderStats);
                UploadDirectionalLights<true>(entities, *voxelConeTracingShader, voxelConeTracingShaderTextureCount,
                                              renderStats, cullModelsAndMeshes);
            }

            {
                TIMER(VctFinalRendering);

                OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
                OGL(glViewport(0, 0, width, height));
                OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
                OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

                voxelConeTracingShader->Bind();

                voxelConeTracingShader->Uniform("V", V);
                voxelConeTracingShader->Uniform("P", P);
                voxelConeTracingShader->Uniform("EyePos", EyePos);
                voxelConeTracingShader->Uniform("time", timeCurrent);
                voxelConeTracingShader->Uniform("ScreenSize", ScreenSize);

                voxelConeTracingShader->Texture("GPositionRadiance", voxelConeTracingShaderTextureCount++,
                                                *positionRadianceTexture->Front());
                voxelConeTracingShader->Texture("GNormalShininess", voxelConeTracingShaderTextureCount++,
                                                *normalShininessTexture->Front());
                voxelConeTracingShader->Texture("GAlbedoSpecular", voxelConeTracingShaderTextureCount++,
                                                *albedoSpecularTexture->Front());
                voxelConeTracingShader->Texture("GDepth", voxelConeTracingShaderTextureCount++,
                                                *depthTexture->Front());

                voxelConeTracingShader->Texture("VoxelizedScene", voxelConeTracingShaderTextureCount++, *voxelTexture);
                voxelConeTracingShader->Uniform("VoxelHalfDimensions", vec3(VCT.halfDimensions));
                voxelConeTracingShader->Uniform("VoxelCenter", VCT.center);

                // todo verify
                voxelConeTracingShader->Uniform("VoxelSize", VCT.halfDimensions / GetActualVoxelTextureSize());

                voxelConeTracingShader->Uniform("DirectMultiplier",
                                                VCT.useDirectLighting ? 1.f : 0.f);
                voxelConeTracingShader->Uniform("IndirectDiffuseMultiplier",
                                                VCT.useIndirectDiffuseLighting ? 1.f : 0.f);
                voxelConeTracingShader->Uniform("IndirectSpecularMultiplier",
                                                VCT.useIndirectSpecularLighting ? 1.f : 0.f);

                renderStats.UniformCalls += 14;

                auto scopedState = gl::ScopedState()
                        .Enable(GL_CULL_FACE)
                        .CullFace(GL_BACK)
                        .Enable(GL_DEPTH_TEST)
                        .Build();

                RenderFullscreenTriangle(renderStats);

                voxelConeTracingShader->Unbind();
            }

            {
                TIMER(VctBlitFboDepth);
                DOLLAR("Deferred (VCT): Blit framebuffer depth")

                framebuffer->Bind(GL_READ_FRAMEBUFFER);

                OGL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
                OGL(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST));
                OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

                framebuffer->Unbind();
            }
        }
    }

    else {
#endif // ENABLE_VOXEL_CONE_TRACING
        {
            TIMER(DeferredAllLightsPass);
            DOLLAR("Deferred: All lights pass")

            OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            OGL(glViewport(0, 0, width, height));
            OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
            OGL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            lightsShader->Bind();

            lightsShader->Uniform("V", V);
            lightsShader->Uniform("P", P);
            lightsShader->Uniform("EyePos", EyePos);
            lightsShader->Uniform("time", timeCurrent);
            lightsShader->Uniform("ScreenSize", ScreenSize);
            lightsShader->Uniform("ZFar", zFar);
            lightsShader->Uniform("ZNear", zNear);

            lightsShader->Texture("GPositionRadiance", lightsShaderTextureCount++, *positionRadianceTexture->Front());
            lightsShader->Texture("GNormalShininess", lightsShaderTextureCount++, *normalShininessTexture->Front());
            lightsShader->Texture("GAlbedoSpecular", lightsShaderTextureCount++, *albedoSpecularTexture->Front());
            lightsShader->Texture("GDepth", lightsShaderTextureCount++, *depthTexture->Front());

            renderStats.UniformCalls += 7;

            auto scopedState = gl::ScopedState()
                    .Enable(GL_CULL_FACE)
                    .CullFace(GL_BACK)
                    .Enable(GL_DEPTH_TEST)
                    .Build();

            RenderFullscreenTriangle(renderStats);

            lightsShader->Unbind();
        }

        {
            TIMER(DeferredBlitFboDepth);
            DOLLAR("Deferred: Blit framebuffer depth")

            framebuffer->Bind(GL_READ_FRAMEBUFFER);

            OGL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            OGL(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST));
            OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

            framebuffer->Unbind();
        }

#ifdef ENABLE_VOXEL_CONE_TRACING
    }
#endif // ENABLE_VOXEL_CONE_TRACING

    {
        TIMER(DeferredRenderSkydome);
        DOLLAR("Deferred: Render skydome")

        // only use rotational part for skydome
        auto const skydomeV = glm::inverse(glm::toMat4(cameraTransform->Quaternion()));

        skydomeShader->Bind();

        skydomeShader->Uniform("EyePos", EyePos);
        skydomeShader->Uniform("time", timeCurrent);
        skydomeShader->Uniform("ScreenSize", ScreenSize);

        renderStats.UniformCalls += 2;

        auto scopedState = gl::ScopedState()
                .Enable(GL_CULL_FACE)
                .CullFace(GL_FRONT)
                .Enable(GL_DEPTH_TEST)
                .Build();

        RenderSkydomes(entities, *skydomeShader, 0, renderStats, P, skydomeV);

        skydomeShader->Unbind();
    }

    if (renderBoundingSpheres) {
        TIMER(DeferredRenderBoundingSpheres);
        DOLLAR("Bounding spheres")

        wireframeShader->Bind();
        wireframeShader->Uniform("V", V);
        wireframeShader->Uniform("P", P);
        wireframeShader->Uniform("EyePos", EyePos);
        wireframeShader->Uniform("time", timeCurrent);
        wireframeShader->Uniform("ScreenSize", ScreenSize);

        renderStats.UniformCalls += 4;

        auto scopedState = gl::ScopedState()
                .Disable(GL_CULL_FACE)
                .Enable(GL_DEPTH_TEST)
                .Build();

        RenderBoundingSpheres(entities, *wireframeShader, 0, renderStats,
                              renderBoundingSpheresAsWireframe);
        wireframeShader->Unbind();
    }

    if (isSnowing) {
        GLenum texCount = 0;

        snowfallParticleShader->Bind();

        snowfallParticleShader->Uniform("P", P);
        snowfallParticleShader->Uniform("V", V);
        snowfallParticleShader->Uniform("EyePos", EyePos);
        snowfallParticleShader->Uniform("time", timeCurrent);
        snowfallParticleShader->Uniform("timeSnowStart", static_cast<float>(timeSnowStart));
        snowfallParticleShader->Uniform("ScreenSize", ScreenSize);

        snowfallParticleShader->Uniform("timeDelta", timeDelta);
        snowfallParticleShader->Texture("SceneDepth", texCount++, *depthTexture->Front());

        snowfallParticleShader->Texture("AmbientSceneLight", texCount++, *voxelTexture);
        snowfallParticleShader->Uniform("VoxelHalfDimensions", vec3(VCT.halfDimensions));
        snowfallParticleShader->Uniform("VoxelCenter", VCT.center);

        UploadDirectionalLights<true>(entities, *snowfallParticleShader, texCount, renderStats, cullModelsAndMeshes);

        {
            auto scopedState = gl::ScopedState()
                    .Enable(GL_BLEND)
                    .BlendFuncAlpha(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
                    .DepthMask(GL_FALSE)
                    .Enable(GL_DEPTH_TEST)
                    .Build();

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

                OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
                OGL(glViewport(0, 0, width, height));

                snowfallParticleShader->Uniform("radius", snow->radius);

                snowfallParticleShader->Texture("InPositionsAngles", texCount + 0, *snow->positionsAngles->Front());
                snowfallParticleShader->Texture("InVelocitiesLifetimes", texCount + 1, *snow->velocitiesLifetimes->Front());

                snowfallParticleShader->Uniform("Count", static_cast<GLint>(snow->count));
                snowfallParticleShader->Uniform("MaxCount", static_cast<GLint>(snow->maxCount));

                OGL(pointMesh->DrawElementsInstanced(static_cast<GLsizei>(snow->count)));
            }
        }

        snowfallParticleShader->Unbind();
    }
}

bool
syst::DeferredRenderer::Serialize(BaseSerializer const &serializer, Json::Value &json, syst::DeferredRenderer &sys)  {
    json["name"] = SystemName;

    SERIALIZE(cfl::syst::DeferredRenderer, json["isSnowing"], sys.isSnowing);

#ifdef ENABLE_VOXEL_CONE_TRACING
    SERIALIZE(cfl::syst::DeferredRenderer, json["useVoxelConeTracing"], sys.useVoxelConeTracing);

    Json::Value &jvoxels = json["voxelConeTracing"];

    SERIALIZE(cfl::syst::DeferredRenderer, jvoxels["textureDimensionExponent"], sys.VCT.textureDimensionExponent);
    SERIALIZE(cfl::syst::DeferredRenderer, jvoxels["volumeHalfDimensions"], sys.VCT.halfDimensions);
    SERIALIZE(cfl::syst::DeferredRenderer, jvoxels["volumeCenter"], sys.VCT.center);
    SERIALIZE(cfl::syst::DeferredRenderer, jvoxels["mipmapLevels"], sys.VCT.mipmapLevels);
    SERIALIZE(cfl::syst::DeferredRenderer, jvoxels["timeBetweenMipmapGeneration"], sys.VCT.timeBetweenMipmapGeneration);

    SERIALIZE(cfl::syst::DeferredRenderer, jvoxels["useDirectVoxelRendering"], sys.VCT.useDirectVoxelRendering);
    Json::Value &jvdirect = jvoxels["directRendering"];

    SERIALIZE(cfl::syst::DeferredRenderer, jvdirect["mipmapLevel"], sys.VCT.DirectRendering.mipmapLevel);
    SERIALIZE(cfl::syst::DeferredRenderer, jvdirect["raymarchSteps"], sys.VCT.DirectRendering.raymarchingSteps);

#endif // ENABLE_VOXEL_CONE_TRACING

    return true;
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

#ifdef ENABLE_VOXEL_CONE_TRACING
    ImGui::Checkbox("Voxel cone tracing", &sys.useVoxelConeTracing);
    if (sys.useVoxelConeTracing) {
        ImGui::DragInt("Mipmap level", &sys.VCT.mipmapLevels, 1, 0, 9);
        ImGui::DragFloat("Mipmap delta time", &sys.VCT.timeBetweenMipmapGeneration, 1, 0, 10);
        ImGui::Checkbox("Compute shader mipmapper", &sys.VCT.useComputeShaderMipmapper);

        ImGui::DragInt("Texture size exponent", &sys.VCT.textureDimensionExponent, 1, 5, 9);
        ImGui::Text("Actual texture size: %i", sys.GetActualVoxelTextureSize());
        ImGui::DragFloat("Half dimensions", &sys.VCT.halfDimensions, 1, 0, std::numeric_limits<float>::max());
        ImGui::DragFloat3("Center", glm::value_ptr(sys.VCT.center), 1);

        ImGui::Checkbox("Direct voxel rendering", &sys.VCT.useDirectVoxelRendering);
        if (sys.VCT.useDirectVoxelRendering) {
            ImGui::DragFloat("Distance", &sys.VCT.DirectRendering.renderDistance, 1, 0,
                             std::numeric_limits<float>::max());
            ImGui::DragInt("Raymarching steps", &sys.VCT.DirectRendering.raymarchingSteps, 1, 0, 1024);
            ImGui::DragInt("Rendered mipmap level", &sys.VCT.DirectRendering.mipmapLevel, 1, 0, sys.VCT.mipmapLevels);
        }
    }

#endif // ENABLE_VOXEL_CONE_TRACING

    ImGui::Checkbox("Cull models and meshes", &sys.cullModelsAndMeshes);
    ImGui::Checkbox("Render bounding spheres", &sys.renderBoundingSpheres);
    if (sys.renderBoundingSpheres) {
        ImGui::Checkbox("- as wireframe", &sys.renderBoundingSpheresAsWireframe);
    }

    ImGui::LabelText("FPS", std::to_string(Time::ComputeFPS()).c_str());
    ImGui::LabelText("ms/frame", std::to_string(Time::ComputeAverageFrametime()).c_str());

    ImGui::Text("Render Stats");
    sys.renderStats.DrawWithImGui();

    ImGui::Text("Timings");
    for (auto const& kvp : sys.durationsByName) {
        ImGui::LabelText(kvp.first.c_str(), "%f ms", 1000 * kvp.second);
    }

    return true;
}
} // namespace cfl
