#pragma once

#include <entityx/Entity.h>
#include <entityx/System.h>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/SystemFactory.hh>
#include <conflagrant/ComponentFactory.hh>

#include <imgui.h>

namespace cfl {
namespace syst {
struct EcsDebugger : public System, public entityx::System<EcsDebugger> {
    inline static string GetName() {
        return "EcsDebugger";
    }

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        ImGui::ShowTestWindow();

        ImGui::Begin("EcsDebugger");

        {
            if (ImGui::CollapsingHeader("Types")) {
                if (ImGui::TreeNode("System Types")) {
                    for (auto const &kvp : SystemFactoriesByName) {
                        ImGui::BulletText("%s", kvp.first.c_str());
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Component Types")) {
                    for (auto const &kvp : ComponentFactoriesByName) {
                        ImGui::BulletText("%s", kvp.first.c_str());
                    }
                    ImGui::TreePop();
                }
            }

            if (ImGui::CollapsingHeader("Scene")) {
                if (ImGui::TreeNode("Active Systems")) {
                    auto &systemManager = *engine->GetSystemManager();
                    for (auto const &kvp : SystemFactoriesByName) {
                        if (kvp.second->HasSystem(systemManager)) {
                            ImGui::BulletText("%s", kvp.first.c_str());
                        }
                    }
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Entities")) {
                    unsigned long entityCount = 0;
                    for (auto entity : entities.entities_for_debugging()) {
                        std::stringstream ss;
                        ss << entityCount++;

                        auto name = entity.component<comp::Name>();
                        if (name) {
                            ss << ": \"" << name->value << "\"";
                        }

                        if (ImGui::TreeNode(ss.str().c_str())) {
                            for (auto const &kvp : ComponentFactoriesByName) {
                                if (kvp.second->HasComponent(entity)) {
                                    ImGui::BulletText("%s", kvp.first.c_str());
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }

        ImGui::End();
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, EcsDebugger &system) {
        return true;
    }
};
} // namespace syst
} // namespace cfl
