#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
class Engine;

typedef double time_t;

class Time final {
    friend class Engine;

    static double currentFrameTime;

    static double previousFrameTime;

public:
    template<typename T = time_t>
    inline static T CurrentTime() {
        return static_cast<T>(currentFrameTime);
    }

    template<typename T = time_t>
    inline static T DeltaTime() {
        return static_cast<T>(currentFrameTime - previousFrameTime);
    }
};
} // namespace cfl
