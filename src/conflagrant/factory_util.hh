#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
template<typename T>
class has_Serialize {
    typedef char one;
    typedef long two;

    template<typename C>
    static one test(decltype(&C::Serialize));

    template<typename C>
    static two test(...);

public:
    enum {
        value = sizeof(test<T>(0)) == sizeof(char)
    };
};

template<typename T>
class has_DrawWithImGui {
    typedef char one;
    typedef long two;

    template<typename C>
    static one test(decltype(&C::DrawWithImGui));

    template<typename C>
    static two test(...);

public:
    enum {
        value = sizeof(test<T>(0)) == sizeof(char)
    };
};
} // namespace cfl
