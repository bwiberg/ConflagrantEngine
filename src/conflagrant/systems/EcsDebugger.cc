#include "EcsDebugger.hh"

#include <conflagrant/SystemFactory.hh>
#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/components/Name.hh>
#include <conflagrant/Engine.hh>

#include <imgui.h>

namespace cfl {
void syst::EcsDebugger::DrawEntityEditor(entityx::Entity &entity) {
    ImGui::Begin("Entity Editor");

    std::stringstream ss;
    ss << "Entity name: ";

    if (entity.has_component<comp::Name>()) {
        auto name = entity.component<comp::Name>();
        ss << name->value;
    } else {
        ss << "(unnamed)";
    }

    ImGui::Text(ss.str().c_str());

    for (auto &kvp : ComponentFactoriesByName) {
        auto const &name = kvp.first;
        auto &factory = *kvp.second;

        if (!factory.HasComponent(entity)) continue;
        if (!ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) continue;

        factory.DrawWithImGui(entity, *input);
    }

    ImGui::End();
}

void syst::EcsDebugger::DrawSystems(std::unordered_set<std::shared_ptr<SystemFactory>> &currentSystems, entityx::SystemManager &manager) {
    ImGui::Begin("Systems");

    for (auto &kvp : SystemFactoriesByName) {
        auto const &name = kvp.first;
        auto &factory = kvp.second;

        if (name == syst::EcsDebugger::GetName()) {
            continue;
        }

        if (!factory->HasSystem(manager)) continue;

        bool isSelected = currentSystems.find(factory) != currentSystems.end();

        ImGui::Selectable(name.c_str(), isSelected);
        if (ImGui::IsItemClicked()) {
            if (isSelected) {
                currentSystems.erase(factory);
            } else {
                currentSystems.insert(factory);
            }
            isSelected = !isSelected;
        }

        if (isSelected) {
            ImGui::Begin(name.c_str());
            if (!factory->DrawWithImGui(manager, *input)) {
                currentSystems.erase(factory);
            }
            ImGui::End();
        }
    }

    ImGui::End();
}

void syst::EcsDebugger::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    auto &systemManager = *engine->GetSystemManager();
    DrawSystems(currentSystems, systemManager);

    ImGui::Begin("Scene", nullptr, ImVec2(300, 400));
    {
        size_t entityCount = 0;
        for (auto entity : entities.entities_for_debugging()) {
            std::stringstream ss;
            ss << entityCount++;

            auto name = entity.component<comp::Name>();
            if (name) {
                ss << ": \"" << name->value << "\"";
            }

            ImGui::Selectable(ss.str().c_str());
            if (ImGui::IsItemClicked()) {
                currentEntity = entity;
            }
        }
    }
    ImGui::End();

    if (currentEntity.valid()) {
        DrawEntityEditor(currentEntity);
    }
}
} // namespace cfl