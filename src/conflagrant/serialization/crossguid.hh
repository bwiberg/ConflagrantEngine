#pragma once

#include "Serialize.hh"
#include <crossguid/Guid.hpp>

namespace cfl {
DEFINE_SERIALIZER_SIGNATURE(xg::Guid);

DEFINE_SERIALIZER(xg::Guid) {
    json = value.str();
    return true;
}

DEFINE_DESERIALIZER(xg::Guid) {
    if (!json.isString()) return false;

    xg::Guid source(json.asCString());
    value.swap(source);

    return true;
}
} // namespace cfl
