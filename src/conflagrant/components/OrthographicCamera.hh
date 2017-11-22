#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>

#include <glm/gtc/matrix_transform.hpp>

namespace cfl {
namespace comp {
struct OrthographicCamera {
    float size, zNear, zFar;

    bool hasChanged{true};

    mat4 projection;

    inline static string const GetName() {
        return "OrthographicCamera";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, OrthographicCamera &camera) {
        SERIALIZE(json["size"], camera.size);
        SERIALIZE(json["near"], camera.zNear);
        SERIALIZE(json["far"], camera.zFar);
        camera.hasChanged = true;
        return true;
    }

    static bool DrawWithImGui(OrthographicCamera &camera) {
        return true;
    }
};
} // namespace comp
} // namespace cfl
