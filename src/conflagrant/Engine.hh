#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/Window.hh>
#include <conflagrant/assets/AssetManager.hh>
#include <entityx/entityx.h>
#include <conflagrant/InputManager.hh>
#include <conflagrant/System.hh>
#include <conflagrant/SystemFactory.hh>

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

    std::shared_ptr<InputManager> input;

    std::shared_ptr<assets::AssetManager> assets;

    bool CreateSystems(Json::Value &json);

    bool CreateEntities(Json::Value &json);

    bool CreateEntity(Json::Value &json);

    bool SaveSystems(Json::Value &json);

    bool SaveEntities(Json::Value &json);

    bool SaveEntity(entityx::Entity &entity, Json::Value &json);

    void ToggleRecording(bool shouldRecord);

    void RecordFrame();

    struct {
        bool isRecording{false};
        int *buffer{nullptr};
        uvec2 size;
        FILE* ffmpeg{nullptr};

        uint const FPS = 60;
    } Recording;

    bool shouldStop;

    bool isGuiEnabled{false};

    std::vector<std::shared_ptr<System>> systemVector;

    std::unique_ptr<Path> currentScenePath{nullptr};

public:
    std::vector<std::shared_ptr<SystemFactory>> orderedSystemFactories;

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

    inline size_t GetNumSystems() const {
        return systemVector.size();
    }

    inline void Stop() {
        shouldStop = true;
    }
};
} // namespace cfl
