#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/glm.hh>

#include <imgui.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace cfl {
namespace comp {
class Transform {
    mat4 matrix;
    bool hasChanged{true};
    vec3 position{0.0f, 0.0f, 0.0f};
    quat rotation{vec3(0, 0, 0)};
    float scale{1.0f};
    vec3 pivot{0.0f, 0.0f, 0.0f};

    inline void UpdateMatrix() {
        if (!hasChanged) {
            return;
        }
        hasChanged = false;

        matrix =
                glm::translate(position) *
                glm::translate(pivot) *
                glm::scale(glm::vec3(scale)) *
                glm::toMat4(rotation) *
                glm::translate(-pivot);
    }

public:
    inline vec3 const &Position() const {
        return position;
    }

    inline void Position(vec3 const &value) {
        position = value;
        hasChanged = true;
    }

    inline quat const &Quaternion() const {
        return rotation;
    }

    inline void Quaternion(quat const &value) {
        rotation = value;
        hasChanged = true;
    }

    inline vec3 EulerAnglesDegrees() const {
        return glm::degrees(glm::eulerAngles(rotation));
    }

    inline void EulerAnglesDegrees(vec3 const &value) {
        rotation = glm::quat(glm::radians(value));
        hasChanged = true;
    }

    inline float Scale() const {
        return scale;
    }

    inline void Scale(float value) {
        scale = value;
    }

    inline vec3 const &Pivot() const {
        return pivot;
    }

    inline void Pivot(vec3 const &value) {
        pivot = value;
        hasChanged = true;
    }

    inline mat4 const &GetMatrix() const {
        $
        const_cast<Transform *>(this)->UpdateMatrix();
        return matrix;
    }

    inline static string const GetName() {
        return "Transform";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Transform &transform) {
        $
        SERIALIZE_CUSTOM(json["pivot"], transform.pivot);
        SERIALIZE_CUSTOM(json["position"], transform.position);
        SERIALIZE_CUSTOM(json["orientation"], transform.rotation);
        SERIALIZE(json["scale"], transform.scale);
        transform.hasChanged = true;
        return true;
    }

    static bool DrawWithImGui(Transform &transform, InputManager const &input) {
        $
        float const DragSpeed = input.GetKey(Key::LEFT_SHIFT) ? 0.01f : 0.5f;

        transform.hasChanged |= ImGui::DragFloat3("Pivot", glm::value_ptr(transform.pivot), DragSpeed);
        transform.hasChanged |= ImGui::DragFloat3("Position", glm::value_ptr(transform.position), DragSpeed);

        vec3 eulerAngles = transform.EulerAnglesDegrees();
        transform.hasChanged |= ImGui::DragFloat3("Euler angles", glm::value_ptr(eulerAngles), DragSpeed);
        transform.EulerAnglesDegrees(eulerAngles);

        transform.hasChanged |= ImGui::DragFloat4("Quaternion", glm::value_ptr(transform.rotation), DragSpeed);
        transform.hasChanged |= ImGui::DragFloat("Scale", &transform.scale, DragSpeed);

        return true;
    }
};
} // namespace comp
} // namespace cfl
