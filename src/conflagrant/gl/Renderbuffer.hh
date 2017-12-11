#pragma once

#include "GlObject.hh"

namespace cfl {
namespace gl {
struct GlRenderbufferFactory {
    inline static void Create(GLuint &x) {
        OGL(glGenRenderbuffers(1, &x));
    };

    inline static void Destroy(GLuint x) {
        OGL(glDeleteRenderbuffers(1, &x));
    };
};

struct Renderbuffer : public GlObject<GlRenderbufferFactory> {
    GLsizei const width{0}, height{0};
    GLenum const internalFormat;

    inline Renderbuffer(Renderbuffer &&o) noexcept
            : GlObject<GlRenderbufferFactory>(std::move(o)),
              width(o.width), height(o.height), internalFormat(o.internalFormat) {}

    inline Renderbuffer(GLsizei width, GLsizei height, GLenum internalFormat)
            : width(width), height(height), internalFormat(internalFormat) {
        Bind();
        OGL(glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height));
        //OGL(glNamedRenderbufferStorage(id, internalFormat, width, height));
        Unbind();
    }

    inline void Bind() const {
        OGL(glBindRenderbuffer(GL_RENDERBUFFER, id));
    }

    inline static void Unbind() {
        OGL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
};
}
}
