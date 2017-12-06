#include "CameraController.hh"
#include "system_util.hh"

#include <conflagrant/math.hh>

namespace cfl {
namespace syst {

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

    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::PerspectiveCamera> perspective;
    ComponentHandle<comp::OrthographicCamera> orthographic;

    auto activeCamera = GetActiveCamera(entities, transform, perspective, orthographic);

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
    if (window->SizeHasChanged(size)) {
        if (perspective) {
            perspective->Size(size);
        }

        if (orthographic) {
            orthographic->Size(size);
        }
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
