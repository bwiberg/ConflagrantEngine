#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <json/json.h>

namespace cfl {

struct Serializer final {
    Serializer() = delete;

    template<typename TValue>
    static bool Serialize(Json::Value &json, TValue &value) {
        json = value;
        return true;
    }
};

struct Deserializer final {
    Deserializer() = delete;

    template<typename TValue>
    static bool Serialize(Json::Value &json, TValue &value) {
        assert(false);
        return false;
    }

    inline static bool Serialize(Json::Value &json, int &value) {
        if (!json.isInt()) return false;
        value = json.asInt();
        return true;
    }

    inline static bool Serialize(Json::Value &json, uint &value) {
        if (!json.isUInt()) return false;
        value = json.asUInt();
        return true;
    }

    inline static bool Serialize(Json::Value &json, float &value) {
        if (!json.isDouble()) return false;
        value = json.asFloat();
        return true;
    }

    inline static bool Serialize(Json::Value &json, double &value) {
        if (!json.isDouble()) return false;
        value = json.asDouble();
        return true;
    }

    inline static bool Serialize(Json::Value &json, bool &value) {
        if (!json.isBool()) return false;
        value = json.asBool();
        return true;
    }

    inline static bool Serialize(Json::Value &json, std::string &value) {
        if (!json.isString()) return false;

        value = json.asString();
        return true;
    }
};

#define DEFINE_SERIALIZER_SIGNATURE(type) template<typename TSerializer> bool Serialize(Json::Value &json, type &value)

#define DEFINE_SERIALIZER(type) template<> bool Serialize<Serializer>(Json::Value &json, type &value)

#define DEFINE_DESERIALIZER(type) template<> bool Serialize<Deserializer>(Json::Value &json, type &value)

#define SERIALIZE(JsonValue, Value) if (!TSerializer::Serialize(JsonValue, Value)) return false
} // namespace cfl
