#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/geometry.hh>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace cfl {
namespace comp {
class PerspectiveCamera {
public:
    static constexpr auto ComponentName = "PerspectiveCamera";

private:
    float fov{60.0f}, zNear{0.01f}, zFar{100.0f};
    uvec2 size{1, 1};

    bool hasChanged{true};

    mat4 projection;
    geometry::Frustum frustum;

    template<typename T>
    inline static T NormCross(T const &a, T const &b) {
        return glm::normalize(glm::cross(a, b));
    }

    inline void UpdateProjectionAndFrustum() {
        using namespace geometry;

        if (!hasChanged) {
            return;
        }
        hasChanged = false;

        projection = glm::perspectiveFov(glm::radians(fov),
                                         static_cast<float>(size.x),
                                         static_cast<float>(size.y),
                                         zNear, zFar);
        vec3 const FarForward = zFar * Forward;
        vec3 const NearForward = zNear * Forward;

        float const FarWidth = glm::tan(glm::radians(fov) / 2) * zFar;
        float const FarHeight = FarWidth * size.y / size.x;

        vec3 const FarTopLeft = FarForward + FarHeight * Up + FarWidth * Left;
        vec3 const FarTopRight = FarForward + FarHeight * Up + FarWidth * Right;
        vec3 const FarBottomLeft = FarForward + FarHeight * Down + FarWidth * Left;
        vec3 const FarBottomRight = FarForward + FarHeight * Down + FarWidth * Right;

        vec3 const Zero{};

        frustum = Frustum{.sides = {
                Plane{.center = NearForward, .normal = Backward},                           // near
                Plane{.center = FarForward, .normal = Forward},                             // far
                Plane{.center = Zero, .normal = NormCross(FarTopLeft, FarTopRight)},        // top
                Plane{.center = Zero, .normal = NormCross(FarTopRight, FarBottomRight)},    // right
                Plane{.center = Zero, .normal = NormCross(FarBottomRight, FarBottomLeft)},  // bottom
                Plane{.center = Zero, .normal = NormCross(FarBottomLeft, FarTopLeft)},      // left
        }};
    }

public:
    inline float Fov() const {
        return fov;
    }

    inline void Fov(float fov) {
        this->fov = fov;
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
        const_cast<PerspectiveCamera *>(this)->UpdateProjectionAndFrustum();
        return projection;
    }

    inline geometry::Frustum const &GetFrustum() const {
        $
        const_cast<PerspectiveCamera *>(this)->UpdateProjectionAndFrustum();
        return frustum;
    }

    inline static bool Serialize(BaseSerializer const& serializer, Json::Value &json,
                                 PerspectiveCamera &camera) {
        SERIALIZE(cfl::comp::PerspectiveCamera, json["fov"], camera.fov);
        SERIALIZE(cfl::comp::PerspectiveCamera, json["zNear"], camera.zNear);
        SERIALIZE(cfl::comp::PerspectiveCamera, json["zFar"], camera.zFar);
        camera.hasChanged |= serializer.IsDeserializer();
        return true;
    }

    static bool DrawWithImGui(PerspectiveCamera &camera, InputManager const &input) {
        float const DragSpeed = (input.GetKey(Key::LEFT_CONTROL) || input.GetKey(Key::LEFT_SHIFT))
                                ? 0.01f : 0.5f;

        float constexpr MinimumDiff = 1e-4f;
        camera.hasChanged |= ImGui::DragFloat("Field of view", &camera.fov, DragSpeed, 1.0f, 120.0f);
        camera.hasChanged |= ImGui::DragFloat("Near clip", &camera.zNear, DragSpeed,
                                              MinimumDiff, camera.zFar - MinimumDiff);
        camera.hasChanged |= ImGui::DragFloat("Far clip", &camera.zFar, DragSpeed,
                                              camera.zNear + MinimumDiff);

        return true;
    }
};
} // namespace comp
} // namespace cfl
