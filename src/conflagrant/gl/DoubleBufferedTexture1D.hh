#pragma once

#include <conflagrant/DoubleBuffer.hh>
#include <conflagrant/gl/Texture.hh>
#include <functional>

namespace cfl {
namespace gl {
class DoubleBufferedTexture1D : public DoubleBuffer<gl::Texture1D> {
public:
    inline DoubleBufferedTexture1D(GLsizei size,
                                   GLenum internalFormat, GLenum format, GLenum type,
                                   bool createMipmap = false)
            : DoubleBuffer(std::move(gl::Texture1D(size, internalFormat, format, type, nullptr, createMipmap)),
                           std::move(gl::Texture1D(size, internalFormat, format, type, nullptr, createMipmap))) {}
};
} // namespace gl
} // namespace cfl
