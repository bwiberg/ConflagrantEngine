#pragma once

#include "GlObject.hh"

namespace cfl {
namespace gl {
struct GlRenderbufferFactory {
    inline static void Create(GLuint &x) { OGL(glGenRenderbuffers(1, &x)); };

    inline static void Destroy(GLuint x) { OGL(glDeleteRenderbuffers(1, &x)); };
};

struct Renderbuffer : public GlObject<GlRenderbufferFactory> {
    GLsizei const width{0}, height{0};
    GLenum const internalFormat;

    inline Renderbuffer(GLenum internalFormat, GLsizei width, GLsizei height)
            : width(width), height(height), internalFormat(internalFormat) {
        OGL(glNamedRenderbufferStorage(id, internalFormat, width, height));
    }

    inline void Bind() const {
        OGL(glBindRenderbuffer(GL_RENDERBUFFER, id));
    }

    inline static void Unbind() const {
        OGL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    }
};
}
}
