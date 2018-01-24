#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/path.hh>

#include <imgui.h>
#include <conflagrant/assets/AssetManager.hh>
#include <conflagrant/assets/Model.hh>

namespace cfl {
namespace comp {
struct Model {
    static constexpr auto ComponentName = "Model";

    std::shared_ptr<assets::Model const> value;
    string path;

    inline static bool Serialize(BaseSerializer const& serializer, Json::Value &json,
                                 Model &model) {
        SERIALIZE(cfl::comp::Model, json, model.path);
        if (serializer.IsDeserializer()) {
            return model.ReloadModel();
        }

        return true;
    }

    inline static bool DrawWithImGui(Model &model, InputManager const &input) {
        size_t constexpr BufferSize = 1024;
        char buf[BufferSize];
        model.path.copy(buf, BufferSize);
        if (ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf))) {
            model.path = string(buf);
        }

        if (ImGui::Button("Reload model")) {
            model.ReloadModel();
        }

        return true;
    }

    inline bool ReloadModel() {
        value = assets::AssetManager::LoadAsset<assets::Model const>(path);
        return value != nullptr;
    }
};
} // namespace comp
} // namespace cfl
