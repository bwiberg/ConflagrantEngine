#include "CameraController.hh"
#include <conflagrant/math.hh>

namespace cfl {
namespace syst {
entityx::Entity GetActiveCamera(entityx::EntityManager &entities) {
    $
    using entityx::ComponentHandle;

    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::ActiveCamera> active;
    ComponentHandle<comp::PerspectiveCamera> perspective;
    ComponentHandle<comp::OrthographicCamera> orthographic;

    entityx::Entity activeCamera;
    for (auto entity : entities.entities_with_components(transform, active, perspective)) {
        activeCamera = entity;
        break;
    }

    if (!activeCamera.valid()) {
        for (auto entity : entities.entities_with_components(transform, active, orthographic)) {
            activeCamera = entity;
            break;
        }
    }

    if (!activeCamera.valid()) {
        activeCamera = entities.create();
        activeCamera.assign<comp::Name>()->value = "(auto-created camera)";
        activeCamera.assign<comp::Guid>();
        activeCamera.assign<comp::Transform>()->Position(vec3(0, 0, 10));
        activeCamera.assign<comp::PerspectiveCamera>();
// activeCamera.assign<comp::OrthographicCamera>();
        activeCamera.assign<comp::ActiveCamera>();
    }

    return activeCamera;
}

void CameraController::MoveCamera(comp::Transform &transform) {
    $
    mat4 const &matrix = transform.GetMatrix();
    vec3 position = transform.Position();

    vec3 const Forward = glm::normalize(mat3(matrix) * vec3(0.0f, 0.0f, -1.0f));
    vec3 const Right = glm::normalize(mat3(matrix) * vec3(1.0f, 0.0f, 0.0f));
    vec3 const Up = glm::normalize(mat3(matrix) * vec3(0.0f, 1.0f, 0.0f));

    float const inputUp = (input->GetKey(Key::W) || input->GetKey(Key::UP)) ? 1.0f : 0.0f;
    float const inputDown = (input->GetKey(Key::S) || input->GetKey(Key::DOWN)) ? 1.0f : 0.0f;
    float const inputRight = (input->GetKey(Key::D) || input->GetKey(Key::RIGHT)) ? 1.0f : 0.0f;
    float const inputLeft = (input->GetKey(Key::A) || input->GetKey(Key::LEFT)) ? 1.0f : 0.0f;

    float const amountForward = inputUp - inputDown;
    float const amountRight = inputRight - inputLeft;

    vec3 const moveForward = amountForward * Forward;
    vec3 const moveSideways = amountRight * Right;

    float speed = input->GetKey(Key::LEFT_SHIFT) ? 4.0f : 1.0f;
    position += static_cast<float>(Time::DeltaTime()) * speed * movementSpeed * (moveForward + moveSideways);
    transform.Position(position);

    dvec2 const delta = input->GetDeltaMousePositionPixel();

    auto orientation = transform.Quaternion();

    auto framePitch = glm::angleAxis(glm::radians(static_cast<float>(turnSpeed * -delta.y)),
                                     vec3(1, 0, 0));
    auto frameYaw = glm::angleAxis(glm::radians(static_cast<float>(turnSpeed * -delta.x)),
                                   vec3(0, 1, 0));

    orientation = glm::normalize(frameYaw * transform.Quaternion() * framePitch);
    transform.Quaternion(orientation);
}

void CameraController::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    $
    using entityx::ComponentHandle;

    auto activeCamera = GetActiveCamera(entities);

    auto transform = activeCamera.component<comp::Transform>();
    auto perspective = activeCamera.component<comp::PerspectiveCamera>();
    auto orthographic = activeCamera.component<comp::OrthographicCamera>();

    if (input->GetKeyDown(Key::F)) {
        isActive = !isActive;
    }

    if (isActive) {
        window->SetCursorMode(CursorMode::HIDDEN_FIXED);
        // update camera
        MoveCamera(*transform);
    } else {
        window->SetCursorMode(CursorMode::NORMAL);
    }

    uvec2 size;
    if (perspective) {
        if (window->SizeHasChanged(size) || perspective->hasChanged) {
            perspective->projection = glm::perspectiveFov(glm::radians(perspective->fov),
                                                          static_cast<float>(size.x),
                                                          static_cast<float>(size.y),
                                                          perspective->zNear, perspective->zFar);
            perspective->hasChanged = false;
        }
    } else if (orthographic) {
        if (window->SizeHasChanged(size) || orthographic->hasChanged) {
            float right = orthographic->size * size.x / 2;
            float left = -right;
            float bottom = orthographic->size * size.y / 2;
            float top = -bottom;

            orthographic->projection = glm::ortho(left, right, bottom, top, orthographic->zNear,
                                                  orthographic->zFar);
            orthographic->hasChanged = false;
        }
    } else {
        LOG_ERROR(cfl::syst::CameraController)
                << "Active camera entity does not have Perspective-/OrthographicCamera" << std::endl;
    }
}

bool CameraController::DrawWithImGui(CameraController &sys, InputManager const &input) {
    $
    ImGui::DragFloat("Movement speed", &sys.movementSpeed, 1.0f, 0);
    ImGui::DragFloat("Turn speed", &sys.turnSpeed, 1.0f, 0);
    return true;
}
} // namespace syst
} // namespace cfl
