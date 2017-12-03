#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/assets/Mesh.hh>

#include <imgui.h>
#include <conflagrant/assets/Texture.hh>
#include <conflagrant/assets/AssetManager.hh>

namespace cfl {
namespace comp {
struct Skydome {
    string texturePath;
    std::shared_ptr<assets::Model> _model;
    std::shared_ptr<assets::Mesh> mesh;
    std::shared_ptr<assets::Texture2D const> texture;

    float radius{50.f}, rotationDegrees{0.f};

    inline static string const &GetName() {
        static const string name = "Skydome";
        return name;
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Skydome &comp) {
        SERIALIZE(json["texturePath"], comp.texturePath);
        return ReloadSkydome(comp);
    }

    inline static bool DrawWithImGui(Skydome &comp, InputManager const &input) {
        ImGui::DragFloat("Radius", &comp.radius, 1.0f, 0.0f);
        ImGui::DragFloat("Rotation", &comp.rotationDegrees, 1.0f, 0.0f);
        return true;
    }

    inline static bool ReloadSkydome(Skydome &skydome) {
        $
        skydome.texture = assets::AssetManager::LoadAsset<assets::Texture2D>(skydome.texturePath);
        skydome._model = assets::AssetManager::LoadAsset<assets::Model>("sphere.obj");
        skydome.mesh = skydome._model->parts[0].first;
        auto &tex = *skydome.texture;
        auto &model = *skydome._model;
        auto &mesh = *skydome.mesh;

        return skydome.texture != nullptr && skydome._model != nullptr && skydome.mesh != nullptr;
    }
};
} // namespace comp
} // namespace cfl
