#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/procgen/parameters/Parameters.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/vector.hh>

namespace cfl {
namespace procgen {
inline bool IsArrayAndAllSameType(Json::Value const &json, Json::ValueType &outValueType) {
    if (!json.isArray() || json.size() < 1) {
        return false;
    }

    Json::ArrayIndex i = 0;
    outValueType = json[i++].type();

    for (; i < json.size(); ++i) {
        if (json[i].type() != outValueType) {
            return false;
        }
    }

    return true;
}

inline bool Serialize(BaseSerializer const &serializer, Json::Value &json, Parameters &value) {
    if (serializer.IsSerializer()) {
        for (auto &kvp : value.strings) SERIALIZE(cfl::procgen::Parameters, json[kvp.first], kvp.second);
        LOG_INFO(cfl::procgen::Parameters) << "No support for bools yet";
        for (auto &kvp : value.ints) SERIALIZE(cfl::procgen::Parameters, json[kvp.first], kvp.second);
        for (auto &kvp : value.floats) SERIALIZE(cfl::procgen::Parameters, json[kvp.first], kvp.second);
    } else {
        if (!json.isObject()) {
            LOG_ERROR(cfl::procgen::Parameters) << "json value should be an object";
            return false;
        }

        auto members = json.getMemberNames();
        for (auto &member : members) {
            auto &jmember = json[member];

            Json::ValueType type;
            if (IsArrayAndAllSameType(jmember, type)) {
                if (type == Json::ValueType::stringValue) {
                    SERIALIZE(cfl::procgen::Parameters, jmember, value.strings[member]);
                } else if (type == Json::ValueType::booleanValue) {
                    LOG_INFO(cfl::procgen::Parameters) << "encountered bool, no support yet";
                } else if (type == Json::ValueType::intValue) {
                    SERIALIZE(cfl::procgen::Parameters, jmember, value.ints[member]);
                } else if (type == Json::ValueType::realValue) {
                    SERIALIZE(cfl::procgen::Parameters, jmember, value.floats[member]);
                } else {
                    LOG_ERROR(cfl::procgen::Parameters)
                            << "encountered array without string/bool/int/float value";
                    return false;
                }
            } else {
                LOG_ERROR(cfl::procgen::Parameters)
                        << "encountered Parameter that wasn't array or had different types of values";
                return false;
            }
        }
    }
}
} // namespace procgen
} // namespace cfl
