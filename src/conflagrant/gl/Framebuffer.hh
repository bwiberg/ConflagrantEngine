#pragma once

#include "GlObject.hh"
#include "Texture.hh"
#include "Renderbuffer.hh"

#include <unordered_map>

namespace cfl {
namespace gl {
struct GlFramebufferFactory {
    inline static void Create(GLuint &x) {
        OGL(glGenFramebuffers(1, &x));
    };

    inline static void Destroy(GLuint x) {
        OGL(glDeleteFramebuffers(1, &x));
    };
};

struct Framebuffer : public GlObject<GlFramebufferFactory> {
    GLsizei const width{0}, height{0};

    std::unordered_map<GLenum, std::shared_ptr<Texture2D>> textureAttachments;
    std::unordered_map<GLenum, std::shared_ptr<Renderbuffer>> renderbufferAttachments;

    inline Framebuffer(Framebuffer &&o) noexcept
            : GlObject<GlFramebufferFactory>(std::move(o)),
              width(o.width), height(o.height),
              textureAttachments(std::move(o.textureAttachments)),
              renderbufferAttachments(std::move(o.renderbufferAttachments)) {}

    inline Framebuffer(GLsizei width, GLsizei height)
            : width(width), height(height) {}

    inline void Attach(GLenum attachment, std::shared_ptr<Texture2D> const &texture2D) {
        textureAttachments[attachment] = texture2D;

        texture2D->Bind();
        OGL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture2D->ID(), 0));
        texture2D->Unbind();
    }

    inline void Attach(GLenum attachment, std::shared_ptr<Renderbuffer> const &renderbuffer) {
        renderbufferAttachments[attachment] = renderbuffer;

        renderbuffer->Bind();
        OGL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbuffer->ID()));
        renderbuffer->Unbind();
    }

    inline void SetDrawBuffer(GLenum buffer) {
        OGL(glNamedFramebufferDrawBuffer(id, buffer));
    }

    inline void SetReadBuffer(GLenum buffer) {
        OGL(glNamedFramebufferReadBuffer(id, buffer));
    }

    inline void SetDrawBuffers(GLsizei N, GLenum const *buffers) {
        OGL(glDrawBuffers(N, buffers));
    }

    inline bool CheckIsComplete() const {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    inline void Bind(GLenum target = GL_FRAMEBUFFER) const {
        OGL(glBindFramebuffer(target, id));
    }

    inline static void Unbind(GLenum target = GL_FRAMEBUFFER) {
        OGL(glBindFramebuffer(target, 0));
    }
};
}
}
