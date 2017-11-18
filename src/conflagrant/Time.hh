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
    inline static time_t CurrentTime() {
        return currentFrameTime;
    }

    inline static time_t DeltaTime() {
        return currentFrameTime - previousFrameTime;
    }
};
} // namespace cfl
