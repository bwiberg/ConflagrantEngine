#include "ForwardRenderer.hh"

#include <conflagrant/Time.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>

#include <imgui.h>

namespace cfl {
namespace syst {
ForwardRenderer::ForwardRenderer() {

}

void RenderModels(entityx::EntityManager &entities) {
    using entityx::ComponentHandle;

    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::Model> model;

    for (auto const& entity : entities.entities_with_components(transform, model)) {
        
    }
}

void ForwardRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt)  {
    uvec2 size = window->GetSize();
    OGL(glViewport(0, 0, size.x, size.y));
    OGL(glClear(GL_COLOR_BUFFER_BIT));
    OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

    RenderModels(entities);
}
} // namespace syst
} // namespace cfl
