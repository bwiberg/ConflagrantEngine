#pragma once

#include "serialize.hh"
#include <crossguid/Guid.hpp>

namespace cfl {
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, xg::Guid &value) {
    if (serializer.IsSerializer()) {
        json = value.str();
    } else {
        if (!json.isString()) return false;

        xg::Guid source(json.asCString());
        value.swap(source);
    }

    return true;
};
} // namespace cfl
