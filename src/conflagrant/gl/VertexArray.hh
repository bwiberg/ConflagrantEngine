#pragma once

#include "GlObject.hh"

namespace cfl {
namespace gl {
struct GlVertexArrayFactory {
    inline static void Create(GLuint &x) { OGL(glGenVertexArrays(1, &x)); };

    inline static void Destroy(GLuint x) { OGL(glDeleteVertexArrays(1, &x)); };
};

struct VertexArray : public GlObject<GlVertexArrayFactory> {
    void Bind() const {
        OGL(glBindVertexArray(id));
    }

    static void Unbind() const {
        OGL(glBindVertexArray(0));
    }
};
}
}
