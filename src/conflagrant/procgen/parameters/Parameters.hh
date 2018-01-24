#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <map>

namespace cfl {
namespace procgen {
template<typename T>
struct ParameterKey {
    using ValueType = T;

    char const *name;

    template<size_t N>
    inline constexpr ParameterKey(char const (&a)[N])
            : name(a) {}
};

using StringParameterKey = ParameterKey<string>;
using BoolParameterKey = ParameterKey<bool>;
using IntParameterKey = ParameterKey<int>;
using FloatParameterKey = ParameterKey<float>;

struct Parameters {
    template<typename T>
    using ParameterList = std::map<string, std::vector<T>>;

    ParameterList<string> strings;
    ParameterList<bool> bools;
    ParameterList<int> ints;
    ParameterList<float> floats;

    inline Parameters() {}

    Parameters(Parameters const &other) = default;

#define OPERATOR(type, konst, function) \
    inline std::vector<type> konst& operator[](ParameterKey<type> const& key) konst { \
        return (type##s).function(key.name); \
    }
#define OPERATORS(type) OPERATOR(type,,operator[]); OPERATOR(type, const, at)

    OPERATORS(string);

    OPERATORS(bool);

    OPERATORS(int);

    OPERATORS(float);
#undef OPERATOR

    template<typename T>
    inline static bool HasSameSpec(ParameterList<T> const &expected,
                                   ParameterList<T> const &actual,
                                   string const &identifier) {
        for (auto const &kvp : expected) {
            auto const &name = kvp.first;
            auto const &values = kvp.second;

            auto it = actual.find(name);
            if (it == actual.end()) {
                LOG_ERROR(cfl::procgen::Parameters::HasSameSpec) << "expected \"" << identifier
                                                                 << "\" to contain \"" << name << "\"";
                return false;
            }

            if (it->second.size() != values.size()) {
                LOG_ERROR(cfl::procgen::Parameters::HasSameSpec) << "expected " << identifier
                                                                 << "[\"" << name
                                                                 << "\"] to have (" << values.size()
                                                                 << ") elements, had (" << it->second.size() << ")";
                return false;
            }
        }

        return true;
    }

    inline static bool HasSameSpec(Parameters const &expected, Parameters const &actual) {
        return HasSameSpec(expected.strings, actual.strings, "strings") &&
               HasSameSpec(expected.bools, actual.bools, "bools") &&
               HasSameSpec(expected.ints, actual.ints, "ints") &&
               HasSameSpec(expected.floats, actual.floats, "floats");
    }
};
} // namespace procgen
} // namespace cfl
