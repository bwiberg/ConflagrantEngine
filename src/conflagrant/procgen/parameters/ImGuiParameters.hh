#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/procgen/parameters/Parameters.hh>

#include <imgui.h>

namespace cfl {
namespace procgen {
inline bool ImGuiEditParameters(Parameters &parameters, bool slide = false) {
    bool hasChanged = false;

    if (!parameters.strings.empty()) {
        LOG_ERROR(cfl::procgen::ImGuiEditParameters) << "string functionality unimplemented";
        return false;
    }

    for (auto &pbool : parameters.bools) {
        auto const &name = pbool.first;
        auto &values = pbool.second;

        if (values.size() != 1) {
            LOG_ERROR(cfl::procgen::ImGuiEditParameters) << "bool can't have multiple values for same name";
            return false;
        }

        ImGui::Text(name.c_str());

        for (size_t i = 0; i < values.size(); ++i) {
            bool value = values[i];
            hasChanged |= ImGui::Checkbox(std::to_string(i).c_str(), &value);
            values[i] = value;
        }
    }

    for (auto &pint : parameters.ints) {
        auto const &name = pint.first;
        auto &values = pint.second;

        auto ptr = &values[0];

        if (values.size() == 1) hasChanged |= (slide ? ImGui::DragInt(name.c_str(), ptr) : ImGui::InputInt(name.c_str(), ptr));
        else if (values.size() == 2) hasChanged |= (slide ? ImGui::DragInt2(name.c_str(), ptr) : ImGui::InputInt2(name.c_str(), ptr));
        else if (values.size() == 3) hasChanged |= (slide ? ImGui::DragInt3(name.c_str(), ptr) : ImGui::InputInt3(name.c_str(), ptr));
        else if (values.size() == 4) hasChanged |= (slide ? ImGui::DragInt4(name.c_str(), ptr) : ImGui::InputInt4(name.c_str(), ptr));
        else {
            LOG_ERROR(cfl::procgen::ImGuiEditParameters) << "int must have between 1-4 values, had " << values.size();
            return false;
        }
    }

    for (auto &pfloat : parameters.floats) {
        auto const &name = pfloat.first;
        auto &values = pfloat.second;

        auto ptr = &values[0];

        if (values.size() == 1) hasChanged |= (slide ? ImGui::DragFloat(name.c_str(), ptr) : ImGui::InputFloat(name.c_str(), ptr));
        else if (values.size() == 2) hasChanged |= (slide ? ImGui::DragFloat2(name.c_str(), ptr) : ImGui::InputFloat2(name.c_str(), ptr));
        else if (values.size() == 3) hasChanged |= (slide ? ImGui::DragFloat3(name.c_str(), ptr) : ImGui::InputFloat3(name.c_str(), ptr));
        else if (values.size() == 4) hasChanged |= (slide ? ImGui::DragFloat4(name.c_str(), ptr) : ImGui::InputFloat4(name.c_str(), ptr));
        else {
            LOG_ERROR(cfl::procgen::ImGuiEditParameters) << "float must have between 1-4 values, had " << values.size();
            return false;
        }
    }

    return hasChanged;
}
} // namespace procgen
} // namespace cfl
