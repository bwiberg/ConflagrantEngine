#include "Time.hh"

namespace cfl {
double Time::currentFrameTime = 0.0;
double Time::previousFrameTime = 0.0;
std::array<time_t, Time::NumFrameTimeSamples> Time::FrameTimeSamples;
size_t Time::CurrentFrameTimeIndex = 0;

void Time::RecordCurrentFrameTime(time_t time) {
    Time::previousFrameTime = Time::currentFrameTime;
    Time::currentFrameTime = time;

    Time::CurrentFrameTimeIndex = (Time::CurrentFrameTimeIndex + 1) % Time::NumFrameTimeSamples;
    Time::FrameTimeSamples[Time::CurrentFrameTimeIndex] = time;
}

void Time::ForceSetDeltaTime(time_t deltaTime) {
    RecordCurrentFrameTime(Time::currentFrameTime + deltaTime);
}

time_t Time::CurrentTime() {
    return (currentFrameTime);
}

time_t Time::DeltaTime() {
    return (currentFrameTime - previousFrameTime);
}

time_t Time::ComputeAverageFrametime(size_t numSamples) {
    assert(numSamples > 1 && numSamples <= NumFrameTimeSamples);

    size_t diff = NumFrameTimeSamples - numSamples + 1;
    size_t subtractIndex = (Time::CurrentFrameTimeIndex + diff) % Time::NumFrameTimeSamples;

    auto current = currentFrameTime;
    auto old = FrameTimeSamples[subtractIndex];
    return (current - old) / (numSamples - 1);
}

time_t Time::ComputeFPS(size_t numSamples) {
    return 1 / ComputeAverageFrametime(numSamples);
}
} // namespace cfl
