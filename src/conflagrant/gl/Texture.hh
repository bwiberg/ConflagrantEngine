#pragma once

#include "GlObject.hh"

namespace cfl {
namespace gl {
struct GlTextureFactory {
    inline static void Create(GLuint &x) { OGL(glGenTextures(1, &x)); };

    inline static void Destroy(GLuint x) { OGL(glDeleteTextures(1, &x)); };
};

struct GlTextureBase : public GlObject<GlTextureFactory> {
    GLenum const internalFormat, format, type, target;
    bool const hasMipmap;

    inline GlTextureBase(GlTextureBase &&o) noexcept
            : GlObject<GlTextureFactory>(std::move(o)),
              target(o.target), internalFormat(o.internalFormat), format(o.format), type(o.type),
              hasMipmap(o.hasMipmap) {}

    inline GlTextureBase(GLenum target, GLenum internalFormat, GLenum format, GLenum type, bool createMipmap = false)
            : target(target), internalFormat(internalFormat), format(format), type(type),
              hasMipmap(createMipmap) {}

    void Bind() const {
        OGL(glBindTexture(target, id));
    }

    void Unbind() const {
        OGL(glBindTexture(target, 0));
    }

    inline void TexParameter(GLenum pname, GLfloat param) {
        OGL(glTextureParameterf(id, pname, param));
    }

    inline void TexParameter(GLenum pname, GLint param) {
        OGL(glTextureParameteri(id, pname, param));
    }

    inline void TexParameter(GLenum pname, GLfloat const *params) {
        OGL(glTextureParameterfv(id, pname, params));
    }

    inline void TexParameter(GLenum pname, GLint const *params) {
        OGL(glTextureParameteriv(id, pname, params));
    }
};

struct Texture1D : public GlTextureBase {
    GLsizei const size;

    inline Texture1D(Texture1D &&o) noexcept
            : GlTextureBase(std::move(o)), size(size) {}

    inline Texture1D(GLsizei size,
                     GLenum internalFormat, GLenum format, GLenum type,
                     GLvoid const *pixels,
                     bool createMipmap = false)
            : GlTextureBase(GL_TEXTURE_1D,
                            internalFormat, format, type), size(size) {
        Bind();

        TexImage1D(pixels);
        TexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        TexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        TexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);

        Unbind();
    }

    inline void TexImage1D(GLvoid const *pixels) {
        OGL(glTexImage1D(target, 0, internalFormat, size, 0, format, type, pixels));
    }
};

struct Texture2D : public GlTextureBase {
    GLsizei const width, height;

    inline Texture2D(Texture2D &&o) noexcept
            : GlTextureBase(std::move(o)), width(width), height(height) {}

    inline Texture2D(GLsizei width, GLsizei height,
                     GLenum internalFormat, GLenum format, GLenum type,
                     GLvoid const *pixels,
                     bool isTextureCube = false, bool createMipmap = false)
            : GlTextureBase(isTextureCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D,
                            internalFormat, format, type),
              width(width), height(height) {
        Bind();

        TexImage2D(pixels);

        TexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        TexParameter(GL_TEXTURE_MIN_FILTER, hasMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        if (!isTextureCube) {
            TexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
            TexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        } else {
            TexParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            TexParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            TexParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            TexParameter(GL_TEXTURE_BASE_LEVEL, 0);
            TexParameter(GL_TEXTURE_MAX_LEVEL, 7);
        }

        Unbind();
    }

    void TexImage2D(GLvoid const *pixels) {
        if (target == GL_TEXTURE_2D) {
            OGL(glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, pixels));
        } else {
            assert(target == GL_TEXTURE_CUBE_MAP);
            for (GLenum i = 0; i < 6; ++i) {
                OGL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format,
                                 type, pixels));
            }
        }
        if (hasMipmap) glGenerateTextureMipmap(id);
    }
};

struct Texture3D : public GlTextureBase {
    GLsizei const width, height, depth;

    inline Texture3D(Texture3D &&o) noexcept
            : GlTextureBase(std::move(o)), width(width), height(height), depth(depth) {}

    inline Texture3D(GLsizei width, GLsizei height, GLsizei depth,
                     GLenum internalFormat, GLenum format, GLenum type,
                     GLvoid const *pixels,
                     bool createMipmap = false)
            : GlTextureBase(GL_TEXTURE_3D, internalFormat, format, type),
              width(width), height(height), depth(depth) {
        Bind();

        TexImage3D(pixels);
        TexParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        TexParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        TexParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
        TexParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
        TexParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

        Unbind();
    }

    void TexImage3D(GLvoid const *pixels) {
        OGL(glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, pixels));
    }
};
}
}
