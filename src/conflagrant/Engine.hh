#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <entityx/entityx.h>

namespace cfl {
class Engine {
    std::shared_ptr<entityx::EntityManager> entities;

    /**
     * Event manager
     */
    std::shared_ptr<entityx::EventManager> events;

    /**
     * ECS system manager
     */
    std::shared_ptr<entityx::SystemManager> systems;

    bool CreateSystems(Json::Value &json);

    bool CreateEntities(Json::Value &json);

    bool CreateEntity(Json::Value &json);

    bool SaveSystems(Json::Value &json);

    bool SaveEntities(Json::Value &json);

    bool SaveEntity(entityx::Entity &entity, Json::Value &json);

public:
    Engine();

    bool LoadScene(Json::Value &json);

    bool LoadScene(std::function<bool(std::shared_ptr<entityx::EntityManager>,
                                      std::shared_ptr<entityx::SystemManager>)> loadSceneFunction);

    bool SaveScene(Json::Value &json);

    int RunOnce();

    int Run();

    inline std::shared_ptr<entityx::EntityManager> GetEntityManager() const {
        return entities;
    }

    inline std::shared_ptr<entityx::EventManager> GetEventManager() const {
        return events;
    }

    inline std::shared_ptr<entityx::SystemManager> GetSystemManager() const {
        return systems;
    }
};
} // namespace cfl
