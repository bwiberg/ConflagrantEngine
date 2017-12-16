#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <chrono>
#include <unordered_map>

namespace cfl {
template<typename time_t>
class Timer {
    time_t &time;
    decltype(std::chrono::system_clock::now()) start;

public:
    inline Timer(time_t &time)
            : time(time) {
        OGL(glFinish());
        start = std::chrono::system_clock::now();
    }

    inline ~Timer() {
        OGL(glFinish());
        std::chrono::duration<time_t> duration = std::chrono::system_clock::now() - start;
        time = duration.count();
    }
};
} // namespace cfl
