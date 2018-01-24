#pragma once

#include "serialize.hh"
#include <vector>

namespace cfl {
template<typename TValue>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, std::vector<TValue> &vector) {
    size_t N;

    if (serializer.IsSerializer()) {
        if (!json.empty()) return false;

        N = vector.size();
        json.resize(static_cast<Json::ArrayIndex>(N));

    } else {
        if (!json.isArray()) return false;

        N = json.size();
        vector.resize(N);
    }

    for (Json::ArrayIndex i = 0; i < N; ++i) {
        SERIALIZE(Serialize<std::vector>, json[i], vector[i]);
    }

    return true;
};
} // namespace cfl
