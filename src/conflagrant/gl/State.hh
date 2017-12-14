#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace gl {
template<bool Enabled>
class State {
    GLenum const capability;
    bool const wasEnabled;

public:
    explicit State(GLenum capability);

    ~State();
};

inline static std::unique_ptr<std::vector<State<true>>> Enable(std::initializer_list<GLenum> capabilities) {
    auto states = std::make_unique<std::vector<State<true>>>();

    for (auto capability : capabilities) {
        states->emplace_back(capability);
    }

    return states;
}

inline static std::unique_ptr<std::vector<State<false>>> Disable(std::initializer_list<GLenum> capabilities) {
    auto states = std::make_unique<std::vector<State<false>>>();

    for (auto capability : capabilities) {
        states->emplace_back(capability);
    }

    return states;
}

template<>
State<true>::State(GLenum capability)
        : capability(capability),
          wasEnabled(static_cast<bool>(glIsEnabled(capability))) {
    if (!wasEnabled) {
        OGL(glEnable(capability));
    }
}

template<>
State<false>::State(GLenum capability)
        : capability(capability),
          wasEnabled(static_cast<bool>(glIsEnabled(capability))) {
    if (wasEnabled) {
        OGL(glDisable(capability));
    }
}

template<>
inline State<true>::~State() {
    if (!wasEnabled) {
        OGL(glDisable(capability));
        return;
    }
}

template<>
inline State<false>::~State() {
    if (wasEnabled) {
        OGL(glEnable(capability));
        return;
    }
}

#define OGL_ENABLE(capability, ...) auto _gl_state_ ## __LINE__ = cfl::gl::Enable({capability, ##__VA_ARGS__})
#define OGL_DISABLE(capability, ...) auto _gl_state_ ## __LINE__ = cfl::gl::Disable({capability, ##__VA_ARGS__})
}
} // namespace cfl
