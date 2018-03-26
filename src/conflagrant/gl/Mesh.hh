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

    inline Mesh(Mesh &&o) noexcept
            : vao(std::move(o.vao)),
              vertexBuffer(std::move(o.vertexBuffer)), indexBuffer(std::move(o.indexBuffer)),
              drawMode(o.drawMode), indexType(o.indexType), vertexStride(o.vertexStride), indexCount(o.indexCount) { }
    inline Mesh(const Mesh &r) = delete;

    inline ~Mesh() = default;

    inline void DrawElements() const {
        if (vertexBuffer.Size() == 0)
            return;

        vao.Bind();

        if (indexCount > 0) {
            indexBuffer.Bind(GL_ELEMENT_ARRAY_BUFFER);
            OGL(glDrawElements(drawMode, static_cast<GLsizei>(indexCount), indexType, nullptr));
        } else {
            OGL(glDrawArrays(drawMode, 0, static_cast<GLsizei>(vertexBuffer.Size() / vertexStride)));
        }

        vao.Unbind();
    }

    inline void DrawElementsInstanced(GLsizei count) const {
        if (vertexBuffer.Size() == 0)
            return;

        vao.Bind();

        if (indexCount > 0) {
            indexBuffer.Bind(GL_ELEMENT_ARRAY_BUFFER);
            OGL(glDrawElementsInstanced(drawMode, static_cast<GLsizei>(indexCount), indexType, nullptr, count));
        } else {
            OGL(glDrawArraysInstanced(drawMode, 0, static_cast<GLsizei>(vertexBuffer.Size() / vertexStride), count));
        }

        vao.Unbind();
    }

    inline void BufferVertexData(GLsizeiptr size, GLvoid const *data, GLenum usage) {
        vertexBuffer.Bind(GL_ARRAY_BUFFER);
        vertexBuffer.BufferData(size, data, usage);
        vertexBuffer.Unbind(GL_ARRAY_BUFFER);
    }

    inline void BufferIndexData(GLenum type, GLsizeiptr count, GLvoid const *data, GLenum usage,
                                GLenum mode = GL_TRIANGLES) {
        size_t size = sizeof_gltype(type);
        indexBuffer.Bind(GL_ELEMENT_ARRAY_BUFFER);
        indexBuffer.BufferData(size * count, data, usage);
        indexBuffer.Unbind(GL_ELEMENT_ARRAY_BUFFER);
        drawMode = mode;
        indexType = type;
        indexCount = count;
    }

    inline void SetDrawMode(GLenum mode) {
        drawMode = mode;
    }

    inline void Attribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
                          GLvoid const *offset) {
        OGL(glEnableVertexArrayAttribEXT(vao.ID(), index));
        OGL(glVertexArrayVertexAttribOffsetEXT(vao.ID(), vertexBuffer.ID(),
                                               index, size, type, normalized, stride, (GLintptr) offset));
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
