#include "Animator.hh"

#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/VelocityAnimation.hh>
#include <conflagrant/components/PeriodicalAnimation.hh>
#include <conflagrant/components/DirectionalLightAnimation.hh>
#include <conflagrant/components/DirectionalLight.hh>
#include <conflagrant/geometry.hh>
#include <conflagrant/math.hh>

namespace cfl {
namespace syst {
void Animator::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    using entityx::ComponentHandle;

    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::VelocityAnimation> velocity;
    ComponentHandle<comp::PeriodicalAnimation> period;

    ComponentHandle<comp::DirectionalLight> light;
    ComponentHandle<comp::DirectionalLightAnimation> lightAnimation;

    auto const delta = static_cast<float>(Time::DeltaTime());

    for (auto entity : entities.entities_with_components(transform, velocity)) {
        if (!velocity->isRunning) {
            continue;
        }

        auto x = glm::angleAxis(delta * velocity->angularVelocity.x, geometry::Right);
        auto y = glm::angleAxis(delta * velocity->angularVelocity.y, geometry::Up);
        auto z = glm::angleAxis(delta * velocity->angularVelocity.z, geometry::Forward);

        auto quat = transform->Quaternion();
        transform->Quaternion(z * y * x * quat);

        transform->Position(transform->Position() + delta * velocity->linearVelocity);
    }

    for (auto entity : entities.entities_with_components(transform, period)) {
        if (!period->isRunning) {
            if (period->wasRunning) {
                transform->Position(period->startPosition);
                transform->Quaternion(period->startRotation);
                period->wasRunning = false;
            }
            continue;
        }

        if (!period->hasSavedStartValues) {
            period->startPosition = transform->Position();
            period->startRotation = transform->Quaternion();
            period->hasSavedStartValues = true;
        }

        auto time = Time::CurrentTime() - period->startTime;
        float t = 0;
        if (period->type == comp::PeriodicalAnimation::Type::SMOOTH) {
            t = static_cast<float>(glm::sin(2 * math::PI_D * time / period->periodSeconds));
        } else {
            time /= period->periodSeconds;
            time += 0.25;

            glm::mod(2 * time, 2.0);

            if (time > 1) {
                time = 2 - time;
            }

            t = static_cast<float>(2 * time - 1);
        }

        transform->Position(period->startPosition + t * period->deltaPosition);

        auto x = glm::angleAxis(t * period->deltaRotation.x, geometry::Right);
        auto y = glm::angleAxis(t * period->deltaRotation.y, geometry::Up);
        auto z = glm::angleAxis(t * period->deltaRotation.z, geometry::Forward);

        transform->Quaternion(z * y * x * period->startRotation);
    }

    for (auto entity : entities.entities_with_components(light, lightAnimation)) {
        if (!lightAnimation->isRunning) {
            continue;
        }

        light->horizontal += delta * lightAnimation->horizontalSpeed;
        light->vertical   += delta * lightAnimation->verticalSpeed;
    }
}

bool Animator::DrawWithImGui(Animator &sys, InputManager const &input) {
    return false;
}
} // namespace syst
} // namespace cfl
