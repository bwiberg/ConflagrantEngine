#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
struct BaseSerializer {
    virtual bool Serialize(Json::Value &json, int &value) const = 0;

    virtual bool Serialize(Json::Value &json, uint &value) const = 0;

    virtual bool Serialize(Json::Value &json, float &value) const = 0;

    virtual bool Serialize(Json::Value &json, double &value) const = 0;

    virtual bool Serialize(Json::Value &json, bool &value) const = 0;

    virtual bool Serialize(Json::Value &json, std::string &value) const = 0;

    virtual bool IsSerializer() const = 0;

    inline bool IsDeserializer() const {
        return !IsSerializer();
    }
};

struct Serializer : public BaseSerializer {
    inline bool Serialize(Json::Value &json, int &value) const override {
        json = value;
        return true;
    }

    inline bool Serialize(Json::Value &json, uint &value) const override {
        json = value;
        return true;
    }

    inline bool Serialize(Json::Value &json, float &value) const override {
        json = value;
        return true;
    }

    inline bool Serialize(Json::Value &json, double &value) const override {
        json = value;
        return true;
    }

    inline bool Serialize(Json::Value &json, bool &value) const override {
        json = value;
        return true;
    }

    inline bool Serialize(Json::Value &json, std::string &value) const override {
        json = value;
        return true;
    }

    bool IsSerializer() const override {
        return true;
    }
};

struct Deserializer : public BaseSerializer {
    inline bool Serialize(Json::Value &json, int &value) const override {
        if (!json.isInt()) return false;
        value = json.asInt();
        return true;
    }

    inline bool Serialize(Json::Value &json, uint &value) const override {
        if (!json.isUInt()) return false;
        value = json.asUInt();
        return true;
    }

    inline bool Serialize(Json::Value &json, float &value) const override {
        if (!json.isDouble()) return false;
        value = json.asFloat();
        return true;
    }

    inline bool Serialize(Json::Value &json, double &value) const override {
        if (!json.isDouble()) return false;
        value = json.asDouble();
        return true;
    }

    inline bool Serialize(Json::Value &json, bool &value) const override {
        if (!json.isBool()) return false;
        value = json.asBool();
        return true;
    }

    inline bool Serialize(Json::Value &json, std::string &value) const override {
        if (!json.isString()) return false;

        value = json.asString();
        return true;
    }

    bool IsSerializer() const override {
        return false;
    }
};

template <typename T>
inline bool Serialize(BaseSerializer const& serializer, Json::Value &json, T &value) {
    return serializer.Serialize(json, value);
}

#define SERIALIZE(LogIdentifier, JsonValue, Value) { \
    if (!cfl::Serialize(serializer, JsonValue, Value)) { \
        LOG_ERROR(LogIdentifier) << "Failed to serialize " << #Value; \
        return false; \
    } \
}
} // namespace cfl
