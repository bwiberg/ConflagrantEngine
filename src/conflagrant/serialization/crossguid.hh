#pragma once

#include "Serialize.hh"
#include <crossguid/Guid.hpp>

namespace cfl {
template<typename TSerializer>
struct _xgGuidSerializerHelper;

template<>
struct _xgGuidSerializerHelper<Serializer> {
    static bool Serialize(Json::Value &json, xg::Guid &value) {
        json = value.str();
        return true;
    }
};

template<>
struct _xgGuidSerializerHelper<Deserializer> {
    static bool Serialize(Json::Value &json, xg::Guid &value) {
        if (!json.isString()) return false;

        xg::Guid source(json.asCString());
        value.swap(source);

        return true;
    }
};

template<typename TSerializer>
static bool Serialize(Json::Value &json, xg::Guid &value) {
    _xgGuidSerializerHelper<TSerializer>::Serialize(json, value);
};
} // namespace cfl
