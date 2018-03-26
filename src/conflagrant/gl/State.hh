#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <list>

namespace cfl {
namespace gl {
namespace detail {
struct State {
    inline virtual ~State() {}

    virtual void Apply() const = 0;

    virtual void Revert() const = 0;
};

struct ScopedStateStack {
    std::list<State *> states;

    inline ~ScopedStateStack() {
        // revert all state changes
        for (auto state : states) {
            state->Revert();
            delete state;
        }
    }
};

struct NullState : State {
    inline void Apply() const override {}

    inline void Revert() const override {}
};

struct EnableState : State {
    GLenum const capability;

    inline EnableState(GLenum capability)
            : capability(capability) {}

    inline void Apply() const override {
        OGL(glEnable(capability));
    }

    inline void Revert() const override {
        OGL(glDisable(capability));
    }
};

struct DisableState : State {
    GLenum const capability;

    inline DisableState(GLenum capability)
            : capability(capability) {}

    inline void Apply() const override {
        OGL(glDisable(capability));
    }

    inline void Revert() const override {
        OGL(glEnable(capability));
    }
};

struct BlendFuncState : State {
    GLint srcPrevious, dstPrevious, src, dst;

    inline BlendFuncState(GLint srcPrevious, GLint dstPrevious,
                          GLint src, GLint dst)
            : srcPrevious(srcPrevious), dstPrevious(dstPrevious),
              src(src), dst(dst) {}

    inline void Apply() const override {
        OGL(glBlendFunc(src, dst));
    }

    inline void Revert() const override {
        OGL(glBlendFunc(srcPrevious, dstPrevious));
    }
};

struct DepthMaskState : State {
    GLboolean previous, current;

    inline DepthMaskState(GLboolean previous, GLboolean current)
            : previous(previous), current(current) {}

    inline void Apply() const override {
        OGL(glDepthMask(current));
    }

    inline void Revert() const override {
        OGL(glDepthMask(previous));
    }
};

struct CullFaceState : State {
    GLenum previous, current;

    inline CullFaceState(GLenum previous, GLenum current)
            : previous(previous), current(current) {}

    inline void Apply() const override {
        OGL(glCullFace(current));
    }

    inline void Revert() const override {
        OGL(glCullFace(previous));
    }
};

struct PolygonModeState : State {
    GLenum face;
    GLenum previous, current;

    inline PolygonModeState(GLenum face, GLenum previous, GLenum current)
            : face(face), previous(previous), current(current) {}

    inline void Apply() const override {
        OGL(glPolygonMode(face, current));
    }

    inline void Revert() const override {
        OGL(glPolygonMode(face, previous));
    }
};

struct ColorMaskState : State {
    GLboolean rp, gp, bp, ap;
    GLboolean rc, gc, bc, ac;

    inline ColorMaskState(
            GLboolean rp, GLboolean gp, GLboolean bp, GLboolean ap,
            GLboolean rc, GLboolean gc, GLboolean bc, GLboolean ac)
            : rp(rp), gp(gp), bp(bp), ap(ap),
              rc(rc), gc(gc), bc(bc), ac(ac) {}

    inline void Apply() const override {
        OGL(glColorMask(rc, gc, bc, ac));
    }

    inline void Revert() const override {
        OGL(glColorMask(rp, gp, bp, ap));
    }
};
}

class ScopedState {
    std::unique_ptr<detail::ScopedStateStack> stack;

public:
    inline ScopedState() : stack(std::make_unique<detail::ScopedStateStack>()) {}

    inline ScopedState &Enable(GLenum capability) {
        OGL(auto isEnabled = glIsEnabled(capability));
        if (!isEnabled) {
            stack->states.push_back(new detail::EnableState(capability));
        }

        return *this;
    }

    inline ScopedState &Disable(GLenum capability) {
        OGL(auto isEnabled = glIsEnabled(capability));
        if (isEnabled) {
            stack->states.push_back(new detail::DisableState(capability));
        }

        return *this;
    }

    inline ScopedState &BlendFuncAlpha(GLint src, GLint dst) {
        GLint srcCurrent;
        GLint dstCurrent;
        OGL(glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcCurrent));
        OGL(glGetIntegerv(GL_BLEND_DST_ALPHA, &dstCurrent));

        if (src != srcCurrent || dst != dstCurrent) {
            stack->states.push_back(new detail::BlendFuncState(
                    srcCurrent, dstCurrent,
                    src, dst
            ));
        }

        return *this;
    }

    inline ScopedState &DepthMask(GLboolean enable) {
        GLboolean current;
        OGL(glGetBooleanv(GL_DEPTH_WRITEMASK, &current));

        if (enable != current) {
            stack->states.push_back(new detail::DepthMaskState(current, enable));
        }

        return *this;
    }

    inline ScopedState &CullFace(GLenum state) {
        GLint temp;
        OGL(glGetIntegerv(GL_CULL_FACE_MODE, &temp));

        auto const current = static_cast<GLenum>(temp);

        if (state != current) {
            stack->states.push_back(new detail::CullFaceState(current, state));
        }

        return *this;
    }

    inline ScopedState &PolygonModeFrontBack(GLenum frontMode, GLenum backMode) {
        GLint previous[2];

        OGL(glGetIntegerv(GL_POLYGON_MODE, previous));
        OGL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

        auto const frontModeCurrent = static_cast<GLenum>(previous[0]);
        auto const backModeCurrent = static_cast<GLenum>(previous[1]);

        if (frontMode != frontModeCurrent) {
            stack->states.push_back(new detail::PolygonModeState(GL_FRONT, frontModeCurrent, frontMode));
        }

        if (backMode != backModeCurrent) {
            stack->states.push_back(new detail::PolygonModeState(GL_FRONT, backModeCurrent, backMode));
        }

        return *this;
    }

    inline ScopedState &ColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a) {
        GLboolean c[4];
        OGL(glGetBooleanv(GL_COLOR_WRITEMASK, c));

        if (r != c[0] ||
            g != c[1] ||
            b != c[2] ||
            a != c[3]) {
            stack->states.push_back(new detail::ColorMaskState(c[0], c[1], c[2], c[3], r, g, b, a));
        }

        return *this;
    }

    inline ScopedState &ColorMask(GLboolean rgba) {
        return ColorMask(rgba, rgba, rgba, rgba);
    }

    inline std::unique_ptr<detail::ScopedStateStack> &&Build() {
        for (auto state : stack->states) {
            state->Apply();
        }

        return std::move(stack);
    }
};
}
} // namespace cfl
