#pragma once

#include "GlObject.hh"
#include "Buffer.hh"
#include "VertexArray.hh"

namespace cfl {
namespace gl {
class Mesh {
    VertexArray vao;
    Buffer vertexBuffer, indexBuffer;

    GLenum drawMode{GL_TRIANGLES};
    GLenum indexType{0};
    GLsizei vertexStride{0};
    GLsizeiptr indexCount{0};

public:
    inline Mesh() = default;

    inline Mesh(const Mesh &r) = delete;

    inline ~Mesh() = default;

    inline void DrawElements() const {
        if (vertexBuffer.Size() > 0) {
            vao.Bind();
            if (indexCount > 0) {
                indexBuffer.Bind(GL_ELEMENT_ARRAY_BUFFER);
                OGL(glDrawElements(drawMode, static_cast<GLsizei>(indexCount), indexType, nullptr));
            } else {
                OGL(glDrawArrays(drawMode, 0, static_cast<GLsizei>(vertexBuffer.Size() / vertexStride)));
            }
            vao.Unbind();
        }
    }

    inline void BufferVertexData(GLsizeiptr size, GLvoid const *data, GLenum usage) {
        vertexBuffer.BufferData(size, data, usage);
    }

    inline void BufferIndexData(GLenum type, GLsizeiptr count, const GLvoid *data, GLenum usage,
                                GLenum mode = GL_TRIANGLES) {
        size_t size = sizeof_gltype(type);
        indexBuffer.BufferData(size * count, data, usage);
        drawMode = mode;
        indexType = type;
        indexCount = count;
    }

    inline void Attribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                          GLvoid const *offset) {
        OGL(glEnableVertexArrayAttrib(vao, index));
        OGL(glVertexAttribPointer(index, size, type, normalized, stride, (GLintptr) offset));
        vertexStride = stride;
    }

    inline void SetIndices(GLenum mode, GLsizeiptr count, uint8_t const *indices, GLenum usage) {
        BufferIndexData(GL_UNSIGNED_BYTE, count, indices, usage, mode);
    }

    inline void SetIndices(GLenum mode, GLsizeiptr count, uint16_t const *indices, GLenum usage) {
        BufferIndexData(GL_UNSIGNED_SHORT, count, indices, usage, mode);
    }

    inline void SetIndices(GLenum mode, GLsizeiptr count, uint32_t const *indices, GLenum usage) {
        BufferIndexData(GL_UNSIGNED_INT, count, indices, usage, mode);
    }
};
}
}
