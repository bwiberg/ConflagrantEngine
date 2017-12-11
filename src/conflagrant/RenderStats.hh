#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <imgui.h>

namespace cfl {
struct RenderStats {
    size_t DrawCalls{0};
    size_t UniformCalls{0};

    size_t Vertices{0};
    size_t Triangles{0};

    size_t PointLights{0};
    size_t DirectionalLights{0};

    size_t ModelsRendered{0};
    size_t ModelsCulled{0};
    size_t MeshesRendered{0};
    size_t MeshesCulled{0};

    inline void Reset() {
        DrawCalls = 0;
        UniformCalls = 0;
        Vertices = 0;
        Triangles = 0;
        PointLights = 0;
        DirectionalLights = 0;
        ModelsRendered = 0;
        ModelsCulled = 0;
        MeshesRendered = 0;
        MeshesCulled = 0;
    }

    inline void DrawWithImGui() const {
        ImGui::LabelText("Draw calls", std::to_string(DrawCalls).c_str());
        ImGui::LabelText("Uniform calls", std::to_string(UniformCalls).c_str());

        ImGui::LabelText("Vertices", std::to_string(Vertices).c_str());
        ImGui::LabelText("Triangles", std::to_string(Triangles).c_str());

        ImGui::LabelText("Point lights", std::to_string(PointLights).c_str());
        ImGui::LabelText("Directional lights", std::to_string(DirectionalLights).c_str());

        ImGui::LabelText("Rendered models", std::to_string(ModelsRendered).c_str());
        ImGui::LabelText("Culled models", std::to_string(ModelsCulled).c_str());
        ImGui::LabelText("Rendered meshes", std::to_string(MeshesRendered).c_str());
        ImGui::LabelText("Culled meshes", std::to_string(MeshesCulled).c_str());
    }
};
} // namespace cfl
