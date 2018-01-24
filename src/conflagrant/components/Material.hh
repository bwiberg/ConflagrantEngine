#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/glm.hh>

#include <conflagrant/assets/Material.hh>
#include <conflagrant/assets/AssetManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct Material {
    static constexpr auto ComponentName = "Material";

    assets::Material value;
    string diffuseTexturePath, specularTexturePath, ambientTexturePath, normalTexturePath;
    bool hasDiffuseTexture{false}, hasSpecularTexture{false}, hasAmbientTexture{false}, hasNormalTexture{false};

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 Material &comp) {
        if (serializer.IsSerializer()) {
            if (comp.hasDiffuseTexture) {
                json["diffuse"] = comp.diffuseTexturePath;
            } else {
                SERIALIZE(cfl::comp::Material, json["diffuse"], comp.value.diffuseColor);
            }

            if (comp.hasSpecularTexture) {
                json["specular"] = comp.specularTexturePath;
            } else {
                SERIALIZE(cfl::comp::Material, json["specular"], comp.value.specularColor);
            }

            if (comp.hasAmbientTexture) {
                json["ambient"] = comp.ambientTexturePath;
            } else {
                SERIALIZE(cfl::comp::Material, json["ambient"], comp.value.ambientColor);
            }

            if (comp.hasNormalTexture) {
                json["normal"] = comp.normalTexturePath;
            }
        } else {
            if (json["diffuse"].type() == Json::stringValue) {
                SERIALIZE(cfl::comp::Material, json["diffuse"], comp.diffuseTexturePath);
            } else if (json["diffuse"].type() == Json::arrayValue) {
                SERIALIZE(cfl::comp::Material, json["diffuse"], comp.value.diffuseColor);
            } else {
                LOG_ERROR(cfl::comp::Material) << "json[\"diffuse\"] is neither string or array";
                return false;
            }

            if (json["specular"].type() == Json::stringValue) {
                SERIALIZE(cfl::comp::Material, json["specular"], comp.specularTexturePath);
            } else if (json["specular"].type() == Json::arrayValue) {
                SERIALIZE(cfl::comp::Material, json["specular"], comp.value.specularColor);
            } else {
                LOG_ERROR(cfl::comp::Material) << "json[\"specular\"] is neither string or array";
                return false;
            }

            if (json["ambient"].type() == Json::stringValue) {
                SERIALIZE(cfl::comp::Material, json["ambient"], comp.ambientTexturePath);
            } else if (json["ambient"].type() == Json::arrayValue) {
                SERIALIZE(cfl::comp::Material, json["ambient"], comp.value.ambientColor);
            } else {
                LOG_ERROR(cfl::comp::Material) << "json[\"ambient\"] is neither string or array";
                return false;
            }

            if (json.isMember("normal")) {
                if (json["normal"].type() == Json::stringValue) {
                    SERIALIZE(cfl::comp::Material, json["normal"], comp.normalTexturePath);
                } else {
                    LOG_ERROR(cfl::comp::Material) << "json[\"normal\"] is not a string";
                    return false;
                }
            }

            comp.ReloadTextures();
        }

        return true;
    }

    inline static bool DrawWithImGui(Material &comp, InputManager const &input) {
        bool hasChanged = false;

        hasChanged |= ImGui::Checkbox("Diffuse Texture", &comp.hasDiffuseTexture);
        if (comp.hasDiffuseTexture) {
            size_t constexpr BufferSize = 1024;
            char buf[BufferSize];
            comp.diffuseTexturePath.copy(buf, BufferSize);
            if (ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf))) {
                comp.diffuseTexturePath = string(buf);
            }
        } else {
            hasChanged |= ImGui::DragFloat3("Diffuse", glm::value_ptr(comp.value.diffuseColor));
        }

        hasChanged |= ImGui::Checkbox("Specular Texture", &comp.hasSpecularTexture);
        if (comp.hasSpecularTexture) {
            size_t constexpr BufferSize = 1024;
            char buf[BufferSize];
            comp.specularTexturePath.copy(buf, BufferSize);
            if (ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf))) {
                comp.specularTexturePath = string(buf);
            }
        } else {
            hasChanged |= ImGui::DragFloat3("Specular", glm::value_ptr(comp.value.specularColor));
        }

        hasChanged |= ImGui::Checkbox("Ambient Texture", &comp.hasAmbientTexture);
        if (comp.hasAmbientTexture) {
            size_t constexpr BufferSize = 1024;
            char buf[BufferSize];
            comp.ambientTexturePath.copy(buf, BufferSize);
            if (ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf))) {
                comp.ambientTexturePath = string(buf);
            }
        } else {
            hasChanged |= ImGui::DragFloat3("Ambient", glm::value_ptr(comp.value.ambientColor));
        }

        hasChanged |= ImGui::Checkbox("Normal Texture", &comp.hasNormalTexture);
        if (comp.hasNormalTexture) {
            size_t constexpr BufferSize = 1024;
            char buf[BufferSize];
            comp.normalTexturePath.copy(buf, BufferSize);
            if (ImGui::InputText("Path", buf, IM_ARRAYSIZE(buf))) {
                comp.normalTexturePath = string(buf);
            }
        }

        if (hasChanged) {
            comp.ReloadTextures();
        }

        return true;
    }

    inline bool ReloadTextures() {
        value.diffuseTexture = hasDiffuseTexture ? assets::AssetManager::LoadAsset<assets::Texture2D>(
                diffuseTexturePath) : nullptr;
        value.specularTexture = hasSpecularTexture ? assets::AssetManager::LoadAsset<assets::Texture2D>(
                specularTexturePath) : nullptr;
        value.ambientTexture = hasAmbientTexture ? assets::AssetManager::LoadAsset<assets::Texture2D>(
                ambientTexturePath) : nullptr;
        value.normalTexture = hasNormalTexture ? assets::AssetManager::LoadAsset<assets::Texture2D>(normalTexturePath)
                                               : nullptr;
    }
};
} // namespace comp
} // namespace cfl
