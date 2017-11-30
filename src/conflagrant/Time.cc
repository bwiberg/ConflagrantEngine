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
} // namespace cfl
