#pragma once

#include "serialize.hh"
#include <conflagrant/types.hh>

namespace cfl {
template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tvec2<T, P> &value) {
    SERIALIZE("Serialize<glm::tvec2<T, P>>", json[0], value[0]);
    SERIALIZE("Serialize<glm::tvec2<T, P>>", json[1], value[1]);

    return true;
};

template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tvec3<T, P> &value) {
    SERIALIZE("Serialize<glm::tvec3<T, P>>", json[0], value[0]);
    SERIALIZE("Serialize<glm::tvec3<T, P>>", json[1], value[1]);
    SERIALIZE("Serialize<glm::tvec3<T, P>>", json[2], value[2]);

    return true;
};

template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tvec4<T, P> &value) {
    SERIALIZE("Serialize<glm::tvec4<T, P>>", json[0], value[0]);
    SERIALIZE("Serialize<glm::tvec4<T, P>>", json[1], value[1]);
    SERIALIZE("Serialize<glm::tvec4<T, P>>", json[2], value[2]);
    SERIALIZE("Serialize<glm::tvec4<T, P>>", json[3], value[3]);

    return true;
};

template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tmat2x2<T, P> &value) {
    SERIALIZE("Serialize<glm::tmat2x2<T, P>>", json[0], value[0][0]);
    SERIALIZE("Serialize<glm::tmat2x2<T, P>>", json[1], value[0][1]);

    SERIALIZE("Serialize<glm::tmat2x2<T, P>>", json[2], value[1][0]);
    SERIALIZE("Serialize<glm::tmat2x2<T, P>>", json[3], value[1][1]);

    return true;
};

template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tmat3x3<T, P> &value) {
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[0], value[0][0]);
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[1], value[0][1]);
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[2], value[0][2]);

    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[3], value[1][0]);
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[4], value[1][1]);
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[5], value[1][2]);

    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[6], value[2][0]);
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[7], value[2][1]);
    SERIALIZE("Serialize<glm::tmat3x3<T, P>>", json[8], value[2][2]);

    return true;
};

template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tmat4x4<T, P> &value) {
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[0], value[0][0]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[1], value[0][1]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[2], value[0][2]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[3], value[0][3]);

    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[4], value[1][0]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[5], value[1][1]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[6], value[1][2]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[7], value[1][3]);

    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[8], value[2][0]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[9], value[2][1]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[10], value[2][2]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[11], value[2][3]);

    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[12], value[3][0]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[13], value[3][1]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[14], value[3][2]);
    SERIALIZE("Serialize<glm::tmat4x4<T, P>>", json[15], value[3][3]);

    return true;
};

template<typename T, glm::precision P>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, glm::tquat<T, P> &quat) {
    SERIALIZE("Serialize<glm::tquat<T, P>>", json[0], quat[0]);
    SERIALIZE("Serialize<glm::tquat<T, P>>", json[1], quat[1]);
    SERIALIZE("Serialize<glm::tquat<T, P>>", json[2], quat[2]);
    SERIALIZE("Serialize<glm::tquat<T, P>>", json[3], quat[3]);

    return true;
};
} // namespace cfl
