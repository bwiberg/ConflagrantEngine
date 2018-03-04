#pragma once

#include <conflagrant/DoubleBuffer.hh>
#include <conflagrant/gl/Texture.hh>
#include <functional>

namespace cfl {
class DoubleBufferedTexture2D : public DoubleBuffer<std::shared_ptr<gl::Texture2D>> {
public:
    inline DoubleBufferedTexture2D(GLsizei width, GLsizei height,
                                   GLenum internalFormat, GLenum format, GLenum type,
                                   bool isTextureCube = false, bool createMipmap = false)
            : DoubleBuffer(
            std::make_shared<gl::Texture2D>(width, height, internalFormat, format, type,
                                            nullptr, isTextureCube, createMipmap),
            std::make_shared<gl::Texture2D>(width, height, internalFormat, format, type,
                                            nullptr, isTextureCube, createMipmap)) {

    }
};
} // namespace cfl
