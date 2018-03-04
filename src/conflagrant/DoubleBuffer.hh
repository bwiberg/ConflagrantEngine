#pragma once

#include <functional>

namespace cfl {
template<typename T>
class DoubleBuffer {
protected:
    std::array<T, 2> values;
    int current{0};

public:
    inline DoubleBuffer(T && front, T && back) {
        values[0] = std::move(front);
        values[1] = std::move(back);
    }

    inline T &Front() {
        return values[current];
    }

    inline T &Back() {
        return values[1 - current];
    }

    inline void Swap() {
        current = 1 - current;
    }

    inline void ApplyToBoth(std::function<void(T&)> functor) {
        functor(values[0]);
        functor(values[1]);
    }
};
} // namespace cfl
