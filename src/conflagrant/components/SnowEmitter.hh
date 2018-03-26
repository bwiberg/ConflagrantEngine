#pragma once

#include <variant>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/math.hh>

#include <conflagrant/gl/Texture.hh>
#include <conflagrant/gl/Framebuffer.hh>
#include <conflagrant/gl/DoubleBufferedTexture2D.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct SnowEmitter {
    static constexpr auto ComponentName = "SnowEmitter";

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 SnowEmitter &comp) {
        SERIALIZE(cfl::comp::Snowfall, json["count"], comp.count);
        SERIALIZE(cfl::comp::Snowfall, json["radius"], comp.radius);
        SERIALIZE(cfl::comp::Snowfall, json["dimensions"], comp.dimensions);
        return true;
    }

    inline static bool DrawWithImGui(SnowEmitter &comp, InputManager const &input) {
        if (ImGui::InputInt("Count", &comp.count))
        {
            comp.count = math::Clamp(comp.count, 0, math::Pow(2, 16));
        }
        ImGui::DragFloat("Radius", &comp.radius, 0.01f, 0.001f, 1.0f);
        ImGui::DragFloat3("Dimensions", glm::value_ptr(comp.dimensions), 0.2f, 0.0f, 25.0f);
        return true;
    }

    int count{0}, maxCount{0};
    float radius{0.1f};
    vec3 dimensions;

    std::shared_ptr<gl::DoubleBufferedTexture2D> positionsAngles, velocitiesLifetimes;
    std::shared_ptr<gl::Framebuffer> framebuffer;

};

inline void InitializeComponent(SnowEmitter &snow) {
    auto const width = static_cast<GLsizei>(snow.maxCount);
    auto const height = static_cast<GLsizei>(1);

    snow.framebuffer = std::make_shared<gl::Framebuffer>(width, height);
    snow.framebuffer->Bind();

    snow.positionsAngles
            = std::make_shared<gl::DoubleBufferedTexture2D>(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    snow.positionsAngles->ApplyToBoth([](std::shared_ptr<gl::Texture2D> &tex) {
        tex->Bind();
        tex->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->Unbind();
    });
    snow.framebuffer->Attach(GL_COLOR_ATTACHMENT0, snow.positionsAngles->Front());

    snow.velocitiesLifetimes
            = std::make_shared<gl::DoubleBufferedTexture2D>(width, height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
    snow.velocitiesLifetimes->ApplyToBoth([](std::shared_ptr<gl::Texture2D> &tex) {
        tex->Bind();
        tex->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        tex->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        tex->Unbind();
    });
    snow.framebuffer->Attach(GL_COLOR_ATTACHMENT1, snow.velocitiesLifetimes->Front());

    snow.framebuffer->Attach(GL_DEPTH_ATTACHMENT,
                             std::make_shared<gl::Renderbuffer>(width, height, GL_DEPTH_COMPONENT));

    if (!snow.framebuffer->CheckIsComplete()) {
        LOG_ERROR(cfl::DeferredRenderer::UpdateFramebuffer) << "Framebuffer incomplete";
    }

}
} // namespace comp
} // namespace cfl
