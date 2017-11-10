#pragma once

#include "Serialize.hh"
#include <array>

namespace cfl {
template<typename TSerializer, typename TValue, size_t N>
struct _ArraySerializerHelper;

template<typename TValue, size_t N>
struct _ArraySerializerHelper<Serializer, TValue, N> {
    static bool Serialize(Json::Value &json, std::array<TValue, N> &array) {
        if (!json.empty()) return false;

        json.resize(static_cast<Json::ArrayIndex>(N));

        for (Json::ArrayIndex i = 0; i < N; ++i) {
            if (!Serializer::Serialize(json[i], array[i])) return false;
        }

        return true;
    }
};

template<typename TValue, size_t N>
struct _ArraySerializerHelper<Deserializer, TValue, N> {
    static bool Serialize(Json::Value &json, std::array<TValue, N> &array) {
        if (!json.isArray() || (json.size() != N)) return false;

        for (Json::ArrayIndex i = 0; i < N; ++i) {
            if (!Deserializer::Serialize(json[i], array[i])) return false;
        }

        return true;
    }
};

template<typename TSerializer, typename TValue, size_t N>
static bool Serialize(Json::Value &json, std::array<TValue, N> &array) {
    _ArraySerializerHelper<TSerializer, TValue, N>::Serialize(json, array);
};
} // namespace cfl
