#pragma once

#include <conflagrant/types.hh>
#include "Serialize.hh"

namespace cfl {
template<typename TSerializer>
struct _PathSerializerHelper;

template<>
struct _PathSerializerHelper<Serializer> {
    static bool Serialize(Json::Value &json, Path &value) {
        json = value.str();
        return true;
    }
};

template<>
struct _PathSerializerHelper<Deserializer> {
    static bool Serialize(Json::Value &json, Path &value) {
        if (json.empty()) return false;
        value.set(json.asString());
        return true;
    }
};

template<typename TSerializer>
static bool Serialize(Json::Value &json, Path &value) {
    _PathSerializerHelper<TSerializer>::Serialize(json, value);
};
} // namespace cfl
