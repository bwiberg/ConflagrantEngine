#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>

namespace cfl {
namespace comp {
struct Name {
    string value;

    friend
    template<typename TSerializer>
    bool Serialize(Json::Value &json, comp::Name &value);
};
} // namespace comp

template<typename TSerializer>
bool Serialize(Json::Value &json, comp::Name &name) {
    SERIALIZE(json, value.value);
}
} // namespace cfl
