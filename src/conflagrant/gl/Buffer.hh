#pragma once

#include "GlObject.hh"

namespace cfl {
namespace gl {
struct GlBufferFactory {
    inline static void Create(GLuint &x) {
        $
        OGL(glGenBuffers(1, &x));
    };

    inline static void Destroy(GLuint x) {
        OGL(glDeleteBuffers(1, &x));
    };
};

class Buffer : public GlObject<GlBufferFactory> {
    GLsizeiptr size{0};

public:
    inline Buffer() = default;

    inline Buffer(Buffer &&o) noexcept
            : GlObject<GlBufferFactory>(std::move(o)), size(o.size) { $ }

    inline void Bind(GLenum target) const {
        $
        OGL(glBindBuffer(target, id));
    }

    inline static void Unbind(GLenum target) {
        $
        OGL(glBindBuffer(target, 0));
    }

    inline GLsizeiptr Size() const {
        $
        return size;
    }

    inline void BufferData(GLsizeiptr size, GLvoid const *data, GLenum usage) {
        $
        OGL(glNamedBufferData(id, this->size = size, data, usage));
    }

    inline void BufferSubData(GLintptr offset, GLsizeiptr size, GLvoid const *data) {
        $
        OGL(glNamedBufferSubData(id, offset, size, data));
    }
};
}
}
