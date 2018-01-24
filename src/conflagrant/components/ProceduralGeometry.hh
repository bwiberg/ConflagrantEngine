#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>
#include <conflagrant/procgen/parameters/Parameters.hh>
#include <conflagrant/procgen/parameters/ImGuiParameters.hh>
#include <conflagrant/procgen/parameters/JsonParameters.hh>
#include <conflagrant/procgen/MeshGenerator.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct ProceduralGeometry {
    static constexpr auto ComponentName = "ProceduralGeometry";

    string generatorName;
    std::shared_ptr<procgen::MeshGenerator> generator;
    procgen::Parameters parameters;

    bool hasChanged{true};
    bool useSlider{false};

    inline bool ResetGeneratorAndParameters() {
        if (generatorName.empty()) {
            return false;
        }

        generator = procgen::MeshGeneratorFactoriesByName[generatorName]->Create();
        parameters = generator->GetDefaultParameters();

        return true;
    }

    inline static bool Serialize(BaseSerializer const &serializer, Json::Value &json,
                                 ProceduralGeometry &comp) {
        SERIALIZE(cfl::comp::ProceduralGeometry, json["type"], comp.generatorName);
        if (serializer.IsDeserializer() && !comp.ResetGeneratorAndParameters()) {
            LOG_ERROR(cfl::comp::ProceduralGeometry) << "Failed to create MeshGenerator (" << comp.generatorName << ")";
            return false;
        }

        procgen::Serialize(serializer, json["parameters"], comp.parameters);
        return true;
    }

    inline static bool DrawWithImGui(ProceduralGeometry &comp, InputManager const &input) {
        if (ImGui::CollapsingHeader("Type")) {
            bool hasChangedGenerator = false;
            for (auto const &kvp : procgen::MeshGeneratorFactoriesByName) {
                auto const &name = kvp.first;

                bool checked = comp.generatorName == name;
                if (ImGui::Checkbox(name.c_str(), &checked) || comp.generatorName.empty()) {
                    comp.hasChanged = true;
                    comp.generatorName = name;
                    hasChangedGenerator = true;
                }
            }

            if (hasChangedGenerator) {
                comp.ResetGeneratorAndParameters();
            }
        }

        if (ImGui::CollapsingHeader("Parameters")) {
            ImGui::Checkbox("Slide values", &comp.useSlider);
            comp.hasChanged |= procgen::ImGuiEditParameters(comp.parameters, comp.useSlider);
        }

        return true;
    }
};
} // namespace comp
} // namespace cfl
