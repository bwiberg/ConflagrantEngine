/**
 * Created by Benjamin Wiberg on 29/10/2017.
 */

#pragma once

#include <memory>
#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace gl {
struct State {
    State(GLenum capability, GLboolean state);

    /**
     * For "CababilityStatePair csp = {GL_CULL_FACE, true}" statements.
     */
    State(std::pair<GLenum, GLboolean> &&pair);

    GLenum capability;
    GLboolean state;
};

State::State(GLenum capability, GLboolean state)
        : capability(capability), state(state) {}

State::State(std::pair<GLenum, GLboolean> &&pair)
        : capability(pair.first), state(pair.second) {}

template<size_t N>
struct StateGroup {
    StateGroup(std::array<GLenum, N> && capabilities);

    StateGroup(std::array<State, N> &&capabilitiesAndStates);

    ~StateGroup();

    void Apply();

    std::array<State, N> capabilitiesAndStates;
};

template<size_t N>
StateGroup::StateGroup(std::array<GLenum, N> && capabilities) {
    for (size_t i = 0; i < N; ++i) {
        capabilitiesAndStates[i].capability = capabilities[i];
        OGL(glGetBooleanv(capabilities[i], &capabilitiesAndStates[i].state));
    }
}

template<size_t N>
StateGroup::StateGroup(std::array<State, N> &&capabilitiesAndStates)
        : capabilitiesAndStates(capabilitiesAndStates) {}

template<size_t N>
StateGroup::~StateGroup() {
    Apply();
}

template<size_t N>
void StateGroup::Apply() {
    for (auto &csp : capabilitiesAndStates) {
        if (csp.state) {
            OGL(glEnable(csp.capability));
        }
        else {
            OGL(glDisable(csp.capability));
        }
    }
}

void herp() {
    StateGroup k = {GL_CULL_FACE, GL_STENCIL_TEST};
}
} // namespace gl
} // namespace cfl
