#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/geometry.hh>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace cfl {
namespace comp {
class OrthographicCamera {
public:
    static constexpr auto ComponentName = "OrthographicCamera";

private:
    float scale{0.001f}, zNear{0.01f}, zFar{100.0f};
    vec2 size;

    bool hasChanged{true};

    mat4 projection;
    geometry::Frustum frustum;

    inline void UpdateProjectionAndFrustum() {
        using namespace geometry;

        if (!hasChanged) {
            return;
        }
        hasChanged = false;

        float const Width = scale * size.x / 2;
        float const Height = scale * size.y / 2;
        projection = glm::ortho(-Width, Width, -Height, Height, zNear, zFar);

        vec3 const FarForward = zFar * Backward;
        vec3 const NearForward = zNear * Backward;

        vec3 const Zero{};

        frustum = Frustum{.sides = {
                Plane{.center = NearForward,          .normal = Forward},     // near
                Plane{.center = FarForward,           .normal = Backward},    // far
                Plane{.center = Up * Height,   .normal = Up},          // top
                Plane{.center = Right * Width, .normal = Right},       // right
                Plane{.center = Down * Height, .normal = Down},        // bottom
                Plane{.center = Left * Width,  .normal = Left},        // left
        }};

        hasChanged = false;
    }

public:
    inline float Scale() const {
        return scale;
    }

    inline void Scale(float scale) {
        this->scale = scale;
        hasChanged = true;
    }

    inline float ZNear() const {
        return zNear;
    }

    inline void ZNear(float zNear) {
        this->zNear = zNear;
        hasChanged = true;
    }

    inline float ZFar() const {
        return zFar;
    }

    inline void ZFar(float zFar) {
        this->zFar = zFar;
        hasChanged = true;
    }

    inline uvec2 Size() const {
        return size;
    }

    inline void Size(uvec2 size) {
        this->size = size;
        hasChanged = true;
    }

    inline mat4 const &GetProjection() const {
        $
        const_cast<OrthographicCamera *>(this)->UpdateProjectionAndFrustum();
        return projection;
    }

    inline geometry::Frustum const &GetFrustum() const {
        $
        const_cast<OrthographicCamera *>(this)->UpdateProjectionAndFrustum();
        return frustum;
    }

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 OrthographicCamera &camera) {
        $
        SERIALIZE(cfl::comp::OrthographicCamera, json["scale"], camera.scale);
        SERIALIZE(cfl::comp::OrthographicCamera, json["near"], camera.zNear);
        SERIALIZE(cfl::comp::OrthographicCamera, json["far"], camera.zFar);
        camera.hasChanged |= serializer.IsDeserializer();
        return true;
    }

    inline static bool DrawWithImGui(OrthographicCamera &camera, InputManager const &input) {
        $
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        camera.hasChanged |= ImGui::DragFloat("Scale", &camera.scale, DragSpeed);
        camera.hasChanged |= ImGui::DragFloat("Near clip", &camera.zNear, DragSpeed);
        camera.hasChanged |= ImGui::DragFloat("Far clip", &camera.zFar, DragSpeed);

        return true;
    }
};
} // namespace comp
} // namespace cfl
