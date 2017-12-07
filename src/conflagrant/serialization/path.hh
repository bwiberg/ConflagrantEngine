#pragma once

#include <conflagrant/types.hh>
#include "serialize.hh"

namespace cfl {
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, Path &value) {
    if (serializer.IsSerializer()) {
        json = value.str();
    } else {
        if (json.empty() || !json.isString()) return false;
        value.set(json.asString());
    }

    return true;
};
} // namespace cfl
