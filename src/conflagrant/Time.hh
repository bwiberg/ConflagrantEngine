#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
class Engine;

class Time final {
    static constexpr size_t NumFrameTimeSamples = 30;

    static std::array<time_t, NumFrameTimeSamples> FrameTimeSamples;

    static size_t CurrentFrameTimeIndex;

    friend class Engine;

    static double currentFrameTime;

    static double previousFrameTime;

    static void RecordCurrentFrameTime(time_t time);

public:
    static time_t CurrentTime();

    static time_t DeltaTime();

    static time_t ComputeAverageFrametime(size_t numSamples = NumFrameTimeSamples);

    static time_t ComputeFPS(size_t numSamples = NumFrameTimeSamples);
};
} // namespace cfl
