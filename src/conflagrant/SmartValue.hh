#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
template<typename T>
class SmartValue {
    std::array<T, 2> values = {T{}, T{}};
    int current{0};

public:
    inline void Swap() {
        current = 1 - current;
    }

    template<typename T>
    friend T const& operator=(SmartValue &smart, T const& value);

    T const& operator()() {
        return values[current];
    }
};

template<typename T>
T const& operator=(SmartValue &smart, T const& value) {
    smart.values[smart.current] = value;
}
} // namespace cfl
