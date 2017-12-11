#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/assets/Mesh.hh>

#include <imgui.h>
#include <conflagrant/assets/Texture.hh>
#include <conflagrant/assets/AssetManager.hh>

namespace cfl {
namespace comp {
struct Skydome {
    static constexpr auto ComponentName = "Skydome";

    string texturePath;
    std::shared_ptr<assets::Model> _model;
    std::shared_ptr<assets::Mesh> mesh;
    std::shared_ptr<assets::Texture2D const> texture;

    float radius{50.f}, rotationDegrees{0.f};

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 Skydome &comp) {
        SERIALIZE(cfl::comp::Skydome, json["texturePath"], comp.texturePath);
        if (serializer.IsDeserializer()) {
            return comp.ReloadSkydome();
        }

        return true;
    }

    inline static bool DrawWithImGui(Skydome &comp, InputManager const &input) {
        ImGui::DragFloat("Radius", &comp.radius, 1.0f, 0.0f);
        ImGui::DragFloat("Rotation", &comp.rotationDegrees, 1.f, 0.f, 360.f);
        return true;
    }

    inline bool ReloadSkydome() {
        $
        texture = assets::AssetManager::LoadAsset<assets::Texture2D>(texturePath);
        _model = assets::AssetManager::LoadAsset<assets::Model>("sphere.obj");
        mesh = _model->parts[0].first;

        return texture != nullptr && _model != nullptr && mesh != nullptr;
    }
};
} // namespace comp
} // namespace cfl
