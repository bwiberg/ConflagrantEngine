#pragma once

#include <entityx/System.h>

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace syst {
struct ForwardRenderer : public cfl::System, public entityx::System<ForwardRenderer> {
    float t{0.0f};

    void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {
        uvec2 size = window->GetSize();
        OGL(glViewport(0, 0, size.x, size.y));

        t += 0.01f;
        float constexpr SPEED = 5.0f;
        OGL(glClear(GL_COLOR_BUFFER_BIT));
        OGL(glClearColor(0.5f + 0.5f * sinf(SPEED * t),
                         0.5f + 0.5f * sinf(SPEED * (t + 2 * M_PI / 3)),
                         0.5f + 0.5f * sinf(SPEED * (t + 2 * 2 * M_PI / 3)),
                         1.0f));
        LOG_INFO(cfl::syst::ForwardRenderer::update) << "Clearing color with t=" << t << "." << std::endl;
    }

    inline static string const GetName() {
        return "ForwardRenderer";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, ForwardRenderer &system) {
        return true;
    }
};
} // namespace syst
} // namespace cfl
