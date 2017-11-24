#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/serialization/path.hh>

#include <imgui.h>
#include <conflagrant/assets/AssetManager.hh>
#include <conflagrant/assets/Model.hh>

namespace cfl {
namespace comp {
struct Model {
    std::shared_ptr<assets::Model const> value;
    string path;

    inline static string const GetName() {
        return "Model";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Model &model) {
        SERIALIZE(json, model.path);

        if (!model.value) {
            model.value = assets::AssetManager::LoadAsset<assets::Model const>(model.path);
        }
        return model.value != nullptr;
    }

    inline static bool DrawWithImGui(Model &model, InputManager const &input) {
        size_t constexpr BufferSize = 1024;
        char buf[BufferSize];
        model.path.copy(buf, BufferSize);
        if (ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf))) {
            model.path = string(buf);
        }

        if (ImGui::Button("Reload model")) {
            ReloadModel(model);
        }

        return true;
    }

    inline static bool ReloadModel(Model &model) {
        model.value = assets::AssetManager::LoadAsset<assets::Model const>(model.path);
        return model.value != nullptr;
    }
};
} // namespace comp
} // namespace cfl
