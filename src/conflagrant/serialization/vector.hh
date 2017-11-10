#pragma once

#include "Serialize.hh"
#include <vector>

namespace cfl {
template<typename TSerializer, typename TValue>
struct _VectorSerializerHelper;

template<typename TValue>
struct _VectorSerializerHelper<Serializer, TValue> {
    static bool Serialize(Json::Value &json, std::vector<TValue> &vector) {
        if (!json.empty()) return false;

        auto const N = vector.size();
        json.resize(static_cast<Json::ArrayIndex>(N));

        for (Json::ArrayIndex i = 0; i < N; ++i) {
            if (!Serializer::Serialize(json[i], vector[i])) return false;
        }

        return true;
    }
};

template<typename TValue>
struct _VectorSerializerHelper<Deserializer, TValue> {
    static bool Serialize(Json::Value &json, std::vector<TValue> &vector) {
        if (!json.isArray() || !vector.empty()) return false;

        auto const N = json.size();
        vector.resize(N);

        for (Json::ArrayIndex i = 0; i < N; ++i) {
            if (!Deserializer::Serialize(json[i], vector[i])) return false;
        }

        return true;
    }
};

template<typename TSerializer, typename TValue>
static bool Serialize(Json::Value &json, std::vector<TValue> &vector) {
    _VectorSerializerHelper<TSerializer, TValue>::Serialize(json, vector);
};
} // namespace cfl
