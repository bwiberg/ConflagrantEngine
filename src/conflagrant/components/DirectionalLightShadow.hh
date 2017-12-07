#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/gl/Framebuffer.hh>
#include <conflagrant/gl/Texture.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct DirectionalLightShadow {
    static constexpr auto ComponentName = "DirectionalLightShadow";

    std::shared_ptr<gl::Framebuffer> framebuffer;
    std::shared_ptr<gl::Texture2D> depthTexture;

    uint width{2048}, height{2048};
    bool hasChanged{true};

    inline bool Reset() {
        framebuffer = std::make_shared<gl::Framebuffer>(static_cast<GLsizei>(width),
                                                        static_cast<GLsizei>(height));
        depthTexture = std::make_shared<gl::Texture2D>(static_cast<GLsizei>(width),
                                                       static_cast<GLsizei>(height),
                                                       GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT,
                                                       nullptr /* no uploaded data, simply allocate gpu memory */);
        depthTexture->Bind();
        depthTexture->TexParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        depthTexture->TexParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        vec4 border{1};
        depthTexture->TexParameter(GL_TEXTURE_BORDER_COLOR, glm::value_ptr(border));
        depthTexture->TexParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        depthTexture->TexParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        framebuffer->Bind();
        framebuffer->Attach(GL_DEPTH_ATTACHMENT, depthTexture);
        framebuffer->SetDrawBuffer(GL_NONE);
        framebuffer->SetReadBuffer(GL_NONE);
        framebuffer->Unbind();

        if (!framebuffer->CheckIsComplete()) {
            LOG_ERROR(cfl::comp::DirectionalLightShadow::Reset) << "Framebuffer incomplete.";
            return false;
        }

        return true;
    }

    inline static bool DrawWithImGui(DirectionalLightShadow &comp, InputManager const &input) {
        ivec2 size(comp.width, comp.height);
        comp.hasChanged |= ImGui::InputInt2("Texture width", glm::value_ptr(size));
        if (comp.hasChanged) {
            comp.width = static_cast<uint>(size.x);
            comp.height = static_cast<uint>(size.y);
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
