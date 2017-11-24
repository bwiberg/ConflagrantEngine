#pragma once

#include <entityx/Entity.h>
#include <entityx/System.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/components/Transform.hh>

namespace cfl {
namespace syst {
struct TransformUpdater : public cfl::System, public entityx::System<TransformUpdater> {
    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        entityx::ComponentHandle<comp::Transform> transform;
        for (auto const &e : entities.entities_with_components(transform)) {
            if (transform->hasChanged) {
                transform->hasChanged;
                transform->matrix =
                        glm::translate(transform->position) *
                        glm::translate(-transform->pivot) *
                        glm::scale(glm::vec3(transform->scale)) *
                        glm::toMat4(transform->orientation) *
                        glm::translate(-transform->pivot);
            }
        }
    }

    inline static string GetName() {
        return "TransformUpdater";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, TransformUpdater &sys) {
        json["name"] = GetName();
        return true;
    }

    static bool DrawWithImGui(TransformUpdater &sys, InputManager const &input) {
        return true;
    }
};
} // namespace syst
} // namespace cfl
