#include "CameraController.hh"
#include "system_util.hh"

#include <conflagrant/math.hh>

namespace cfl {
namespace syst {

void CameraController::MoveCamera(comp::Transform &transform) {
    $
    mat4 const &matrix = transform.GetMatrix();
    vec3 position = transform.Position();

    vec3 const CamForward = glm::normalize(mat3(matrix) * geometry::Backward);
    vec3 const CamRight = glm::normalize(mat3(matrix) * geometry::Right);
    vec3 const CamUp = glm::normalize(mat3(matrix) * geometry::Up);

    float const InputUp = (input->GetKey(Key::W) || input->GetKey(Key::UP)) ? 1.0f : 0.0f;
    float const InputDown = (input->GetKey(Key::S) || input->GetKey(Key::DOWN)) ? 1.0f : 0.0f;
    float const InputRight = (input->GetKey(Key::D) || input->GetKey(Key::RIGHT)) ? 1.0f : 0.0f;
    float const InputLeft = (input->GetKey(Key::A) || input->GetKey(Key::LEFT)) ? 1.0f : 0.0f;

    float const AmountForward = InputUp - InputDown;
    float const AmountRight = InputRight - InputLeft;

    vec3 const MoveForward = AmountForward * CamForward;
    vec3 const MoveSideways = AmountRight * CamRight;

    float const Speed = input->GetKey(Key::LEFT_SHIFT) ? 4.0f : 1.0f;
    position += static_cast<float>(Time::DeltaTime()) * Speed * movementSpeed * (MoveForward + MoveSideways);
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
