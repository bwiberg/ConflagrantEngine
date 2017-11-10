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

    friend
    template<typename TSerializer>
    bool Serialize(Json::Value &json, comp::Guid &guid);
};
} // namespace comp

template<typename TSerializer>
bool Serialize(Json::Value &json, comp::Guid &guid) {
    SERIALIZE(json, guid);
}

} // namespace cfl
