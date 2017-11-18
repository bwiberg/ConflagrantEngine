#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <crossguid/Guid.hpp>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/crossguid.hh>

namespace cfl {
namespace comp {
struct Guid {
    xg::Guid value;

    inline static string const GetName() {
        return "Guid";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, comp::Guid &guid) {
        return cfl::Serialize<TSerializer>(json, guid.value);
    }
};
} // namespace comp
} // namespace cfl
