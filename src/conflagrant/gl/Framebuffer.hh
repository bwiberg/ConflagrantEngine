#pragma once

#include "GlObject.hh"
#include "Texture.hh"
#include "Renderbuffer.hh"

namespace cfl {
namespace gl {
struct GlFramebufferFactory {
    inline static void Create(GLuint &x) {
        $
        OGL(glGenFramebuffers(1, &x));
    };

    inline static void Destroy(GLuint x) {
        $
        OGL(glDeleteFramebuffers(1, &x));
    };
};

struct Framebuffer : public GlObject<GlFramebufferFactory> {
    GLsizei const width{0}, height{0};
    Texture2D colorTexture;
    Renderbuffer depthStencilRb; // depth and stencil renderbuffer (will NOT be sampled => renderbuffer > texture)

    inline Framebuffer(Framebuffer &&o) noexcept
            : GlObject<GlFramebufferFactory>(std::move(o)),
              width(o.width), height(o.height), colorTexture(std::move(colorTexture)),
              depthStencilRb(std::move(depthStencilRb)) { $ }

    inline Framebuffer(GLsizei width, GLsizei height)
            : width(width), height(height),
              colorTexture(width, height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, NULL, false, false),
              depthStencilRb(GL_DEPTH24_STENCIL8, width, height) {
        $

        Bind();

        // configure RGB color texture
        colorTexture.Bind();
        colorTexture.TexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        colorTexture.TexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        OGL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture.id, 0));

        // configure depth and stencil renderbuffer
        depthStencilRb.Bind();
        OGL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRb.id));

        // check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOG_ERROR(cfl::gl::Framebuffer::Framebuffer()) << "fbo is incomplete" << std::endl;
        }

        Unbind();
    }

    inline void Bind() const {
        $
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, id));
    }

    inline static void Unbind() const {
        $
        OGL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
};
}
}
