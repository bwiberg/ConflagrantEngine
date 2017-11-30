#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
class Engine;

typedef double time_t;

class Time final {
    static constexpr size_t NumFrameTimeSamples = 30;

    static std::array<time_t, NumFrameTimeSamples> FrameTimeSamples;

    static size_t CurrentFrameTimeIndex;

    friend class Engine;

    static double currentFrameTime;

    static double previousFrameTime;

    static void RecordCurrentFrameTime(time_t time);

public:
    template<typename T = time_t>
    inline static T CurrentTime() {
        return static_cast<T>(currentFrameTime);
    }

    template<typename T = time_t>
    inline static T DeltaTime() {
        return static_cast<T>(currentFrameTime - previousFrameTime);
    }

    template<typename T = time_t>
    inline static T ComputeFPS(size_t numSamples = NumFrameTimeSamples) {
        assert(numSamples > 1 && numSamples <= NumFrameTimeSamples);

        size_t diff = NumFrameTimeSamples - numSamples + 1;
        size_t subtractIndex = (Time::CurrentFrameTimeIndex + diff) % Time::NumFrameTimeSamples;

        auto current = currentFrameTime;
        auto old = FrameTimeSamples[subtractIndex];
        return numSamples / (current - old);
    }
};
} // namespace cfl
