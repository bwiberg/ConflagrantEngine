#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/Window.hh>
#include <conflagrant/assets/AssetManager.hh>
#include <entityx/entityx.h>
#include <conflagrant/Input.hh>
#include <conflagrant/System.hh>

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

    std::shared_ptr<Window> const window;

    std::shared_ptr<Input> input;

    std::shared_ptr<assets::AssetManager> assets;

    bool CreateSystems(Json::Value &json);

    bool CreateEntities(Json::Value &json);

    bool CreateEntity(Json::Value &json);

    bool SaveSystems(Json::Value &json);

    bool SaveEntities(Json::Value &json);

    bool SaveEntity(entityx::Entity &entity, Json::Value &json);

    bool shouldStop;

    std::vector<std::shared_ptr<System>> systemVector;

public:
    explicit Engine(std::shared_ptr<Window> window = nullptr);

    ~Engine();

    bool LoadScene(string const &pathToJson);

    bool LoadScene(Json::Value &json);

    bool LoadScene(std::function<bool(std::shared_ptr<entityx::EntityManager>,
                                      std::shared_ptr<entityx::SystemManager>)> loadSceneFunction);

    bool SaveScene(Json::Value &json);

    bool UnloadScene();

    int Run(bool singleTimestep = false);

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
