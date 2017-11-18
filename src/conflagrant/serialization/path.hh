#pragma once

#include "Serialize.hh"
#include <conflagrant/types.hh>

namespace cfl {
DEFINE_SERIALIZER_SIGNATURE(cfl::Path);

DEFINE_SERIALIZER(cfl::Path) {
    json = value.str();
    return true;
}

DEFINE_DESERIALIZER(cfl::Path) {
    if (json.empty()) return false;
    value.set(json.asString());
    return true;
}
} // namespace cfl
