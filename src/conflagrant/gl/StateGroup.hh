/**
 * Created by Benjamin Wiberg on 29/10/2017.
 */

#pragma once

#include <memory>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace gl {
struct PushedState {
    inline PushedState(const uint id) : id(id) {}

    ~PushedState();

    uint const id;
};

class StateStack {
    friend struct PushedState;

    struct State {
        inline State(GLenum capability, bool state)
                : capability(capability), state(state) {}

        inline State(std::pair<GLenum, bool> &&pair)
                : State(pair.first, pair.second) {}

        inline void Apply() const {
            if (state) {
                OGL(glEnable(capability));
            } else {
                OGL(glDisable(capability));
            }
        }

        GLenum capability;
        bool state;
    };

    static std::stack<std::unordered_set<State>> stack;

    inline static void Pop() {
        stack.pop();
        auto &set = stack.top();
        for (auto &state : set) {
            state.Apply();
        }
    }

    inline static void PopUntilId(uint id) {
        while (stack.size() > id) {
            Pop();
        }
    }

public:
    StateStack() = delete;

    inline static PushedState Push(std::unordered_map<GLenum, bool> stateMap) {
        uint id = static_cast<uint>(stack.size());

        stack.emplace();
        auto &set = stack.top();
        for (auto const &statePair : stateMap) {
            GLenum capability = statePair.first;
            bool active = statePair.second;

            State state(capability, active);
            state.Apply();
            set.insert(state);
        }

        return PushedState(id);
    }
};

inline PushedState::~PushedState() {
    StateStack::PopUntilId(id);
}

template<size_t N>
struct StateGroup {
    StateGroup(std::array<GLenum, N> &&capabilities);

    StateGroup(std::array<State, N> &&capabilitiesAndStates);

    ~StateGroup();

    void Apply();

    std::array<State, N> capabilitiesAndStates;
};

template<size_t N>
StateGroup::StateGroup(std::array<GLenum, N> &&capabilities) {
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
        } else {
            OGL(glDisable(csp.capability));
        }
    }
}

void herp() {
    StateGroup k = {{GL_CULL_FACE, GL_STENCIL_TEST}};
}
} // namespace gl
} // namespace cfl
