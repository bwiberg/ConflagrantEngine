#pragma once

#include "serialize.hh"
#include <array>

namespace cfl {
template<typename TValue, size_t N>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, std::array<TValue, N> &array) {
    if (serializer.IsSerializer()) {
        if (!json.isArray() || (json.size() != N)) return false;
    } else {
        if (!json.empty()) return false;
        json.resize(static_cast<Json::ArrayIndex>(N));
    }

    for (Json::ArrayIndex i = 0; i < N; ++i) {
        SERIALIZE(Serialize<std::array>, json[i], array[i]);
    }

    return true;
};
} // namespace cfl
