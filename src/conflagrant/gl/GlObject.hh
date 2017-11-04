/**
 * Created by Benjamin Wiberg on 29/10/2017.
 */

#pragma once

#include <iostream>
#include <memory>
#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace gl {
template<typename TFactory>
struct GlObject {
    inline GlObject() {
        TFactory::Create(id);
    }

    GlObject(GlObject<TFactory> const &other) = delete;

    GlObject &operator=(GlObject<TFactory> const &other) = delete;

    inline ~GlObject() {
        TFactory::Destroy(id);
    }

    inline GLuint ID() const {
        return id;
    }

    inline operator GLuint() const {
        return id;
    }

protected:
    GLuint id;
};

inline size_t sizeof_gltype(GLenum type) {
    switch (type) {
        case GL_UNSIGNED_BYTE:
            return sizeof(uint8_t);
        case GL_UNSIGNED_SHORT:
            return sizeof(uint16_t);
        case GL_UNSIGNED_INT:
            return sizeof(uint32_t);
        default:
            throw std::logic_error("invalid GL type, should be one of GL_UNSIGNED_[BYTE, SHORT, INT]");
    }
}
}
}
