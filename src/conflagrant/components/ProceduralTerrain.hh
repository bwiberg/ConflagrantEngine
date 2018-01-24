#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/serialization/glm.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct ProceduralTerrain {
    static constexpr auto ComponentName = "ProceduralTerrain";

    vec2 worldSize;
    uvec2 meshDimensions;
    float maxHeight, noiseScale;
    int seed{0};

    bool hasChanged{true};

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json, ProceduralTerrain &comp) {
        SERIALIZE(cfl::comp::ProceduralTerrain, json["worldSize"], comp.worldSize);
        SERIALIZE(cfl::comp::ProceduralTerrain, json["meshDimensions"], comp.meshDimensions);
        SERIALIZE(cfl::comp::ProceduralTerrain, json["maxHeight"], comp.maxHeight);
        SERIALIZE(cfl::comp::ProceduralTerrain, json["noiseScale"], comp.noiseScale);
        SERIALIZE(cfl::comp::ProceduralTerrain, json["seed"], comp.seed);

        comp.hasChanged |= serializer.IsDeserializer();

        return true;
    }

    inline static bool DrawWithImGui(ProceduralTerrain &comp, InputManager const &input) {
        comp.hasChanged |= ImGui::DragFloat2("worldSize", glm::value_ptr(comp.worldSize));

        ivec2 meshDimensions = comp.meshDimensions;
        comp.hasChanged |= ImGui::DragInt2("meshDimensions", glm::value_ptr(meshDimensions), 1.0f, 1, 1000);
        comp.meshDimensions = meshDimensions;

        comp.hasChanged |= ImGui::DragFloat("maxHeight", &comp.maxHeight);
        comp.hasChanged |= ImGui::DragFloat("noiseScale", &comp.noiseScale);

        comp.hasChanged = ImGui::DragInt("Seed", &comp.seed);
        if (ImGui::Button("Randomize seed")) {
            comp.hasChanged = true;
            comp.seed = rand();
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
