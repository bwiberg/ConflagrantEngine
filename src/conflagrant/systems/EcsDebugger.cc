#include "EcsDebugger.hh"
#include "system_util.hh"

#include <conflagrant/SystemFactory.hh>
#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/components/Name.hh>
#include <conflagrant/Engine.hh>
#include <conflagrant/math.hh>

#include <imgui.h>
#include <algorithm>

namespace cfl {
syst::EcsDebugger::EcsDebugger() {
    LoadShaders();
}

void syst::EcsDebugger::LoadShaders() {

}

void syst::EcsDebugger::DrawEntityEditor(entityx::Entity &entity) {
    $
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

    std::vector<string> componentNames;

    for (auto &kvp : ComponentFactoriesByName) {
        auto const &name = kvp.first;
        auto &factory = *kvp.second;

        if (!factory.IsImGuiDrawable()) continue;
        if (!factory.HasComponent(entity)) continue;

        componentNames.emplace_back(factory.GetName());

        if (!ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) continue;

        factory.DrawWithImGui(entity, *input);
    }

    if (ImGui::CollapsingHeader("Add component")) {
        for (auto &kvp : ComponentFactoriesByName) {
            auto const &name = kvp.first;
            auto &factory = *kvp.second;

            if (std::find(componentNames.begin(), componentNames.end(), name) != componentNames.end()) {
                // entity already has component
                continue;
            }

            if (ImGui::Button(name.c_str())) {
                factory.Create(entity);
            }
        }
    }

    ImGui::End();
}

void syst::EcsDebugger::DrawSystems(std::unordered_set<std::shared_ptr<SystemFactory>> &currentSystems,
                                    entityx::SystemManager &manager) {
    $
    std::shared_ptr<SystemFactory> toMove = nullptr;
    bool moveDown = false;

    ImGui::Begin("Systems");

    for (auto &factory : engine->orderedSystemFactories) {
        if (!factory->IsImGuiDrawable()) continue;
        if (!factory->HasSystem(manager)) continue;

        auto const &name = factory->GetName();

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
            if (ImGui::Button("Move up")) {
                toMove = factory;
            } else if (ImGui::Button("Move down")) {
                toMove = factory;
                moveDown = true;
            }

            if (!factory->DrawWithImGui(manager, *input)) {
                currentSystems.erase(factory);
            }
            ImGui::End();
        }
    }

    ImGui::End();

    if (!toMove) return;

    auto iter = std::find(engine->orderedSystemFactories.begin(), engine->orderedSystemFactories.end(), toMove);
    if (moveDown) {
        if (iter + 1 == engine->orderedSystemFactories.end()) {
            return;
        }

        std::iter_swap(iter, iter + 1);
        return;
    } else {
        // move up
        if (iter == engine->orderedSystemFactories.begin()) {
            return;
        }

        std::iter_swap(iter, iter - 1);
        return;
    }
}

void syst::EcsDebugger::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    $
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

        if (ImGui::Button("Create entity")) {
            currentEntity = entities.create();

            auto guid = currentEntity.assign<comp::Guid>();
            auto name = currentEntity.assign<comp::Name>();
            name->value = "(new entity)";
        }
    }
    ImGui::End();

    if (currentEntity.valid()) {
        DrawEntityEditor(currentEntity);
    }
}
} // namespace cfl
