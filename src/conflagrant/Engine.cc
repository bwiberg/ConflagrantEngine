#include "Engine.hh"

#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/SystemFactory.hh>

namespace cfl {
Engine::Engine() {}

bool Engine::LoadScene(Json::Value &json) {
    events = std::make_shared<entityx::EventManager>();
    entities = std::make_shared<entityx::EntityManager>(*events);
    systems = std::make_shared<entityx::SystemManager>(*entities, *events);

    if (!json.isObject()) {
        // todo error message
        return false;
    }

    if (json.isMember("systems") && !CreateSystems(json["systems"])) {
        // todo error message
        return false;
    }

    if (json.isMember("entities") && !CreateEntities(json["entities"])) {
        // todo error message
        return false;
    }

    systems->configure();
    return true;
}

bool Engine::LoadScene(std::function<bool(std::shared_ptr<entityx::EntityManager>,
                                  std::shared_ptr<entityx::SystemManager>)> loadSceneFunction) {
    events = std::make_shared<entityx::EventManager>();
    entities = std::make_shared<entityx::EntityManager>(*events);
    systems = std::make_shared<entityx::SystemManager>(*entities, *events);

    if (!loadSceneFunction(entities, systems)) {
        // todo error message
        return false;
    }

    systems->configure();
    return true;
}

bool Engine::CreateSystems(Json::Value &json) {
    if (!json.isArray()) {
        // todo error message
        return false;
    }

    Json::ArrayIndex const NumSystems = json.size();
    for (Json::ArrayIndex i = 0; i < NumSystems; ++i) {
        Json::Value &jsonSystem = json[i];

        std::string systemName = jsonSystem["name"].asString();

        auto it = SystemFactoriesByName.find(systemName);
        if (it == SystemFactoriesByName.end()) {
            // todo error message
            return false;
        }

        std::shared_ptr<SystemFactory> factory = it->second;
        if (!factory->Create(*systems, jsonSystem)) {
            // todo error message
            return false;
        }
    }

    return true;
}

bool Engine::CreateEntities(Json::Value &jsonEntities) {
    if (!jsonEntities.isArray()) {
        // todo error message
        return false;
    }

    Json::ArrayIndex const NumEntities = jsonEntities.size();
    for (Json::ArrayIndex i = 0; i < NumEntities; ++i) {
        if (!CreateEntity(jsonEntities[i])) {
            // todo error message
            return false;
        }
    }

    return true;
}

bool Engine::CreateEntity(Json::Value &jsonEntity) {
    entityx::Entity entity = entities->create();
#define RETURN_ERROR() { entity.destroy(); return false; }

    if (!jsonEntity.isObject()) {
        // todo error message
        RETURN_ERROR();
    }

    // loop through each json object member and try to create a component matching the member's name
    Json::Value::Members const &componentNames = jsonEntity.getMemberNames();
    for (auto const &componentName : componentNames) {
        auto it = ComponentFactoriesByName.find(componentName);
        if (it == ComponentFactoriesByName.end()) {
            // todo error message
            RETURN_ERROR();
        }

        if (!it->second->Create(entity, jsonEntity[componentName])) {
            // todo error message
            RETURN_ERROR();
        }
    }

    return true;
}

int Engine::Run() {

}

int Engine::RunOnce() {
    systems->update_all(0);
}

// save scene

bool Engine::SaveScene(Json::Value &json) {
    if (!SaveSystems(json["systems"])) {
        // todo error message
        return false;
    }

    if (!SaveEntities(json["entities"])) {
        // todo error message
        return false;
    }

    return true;
}

bool Engine::SaveSystems(Json::Value &json) {
    for (auto const &pair : SystemFactoriesByName) {
        auto const &name = pair.first;
        auto const &factory = *pair.second;

        if (!factory.HasSystem(*systems)) {
            continue;
        }

        Json::Value jsonSystem;
        if (!factory.Serialize(jsonSystem, *systems)) {
            // todo error message
            return false;
        }

        json.append(jsonSystem);
    }

    return true;
}

bool Engine::SaveEntities(Json::Value &json) {
    auto allEntities = entities->entities_for_debugging();

    for (auto entity : allEntities) {
        Json::Value jsonEntity;
        if (!SaveEntity(entity, jsonEntity)) {
            // todo error message
            return false;
        }
        json.append(jsonEntity);
    }

    return true;
}

bool Engine::SaveEntity(entityx::Entity &entity, Json::Value &json) {
    for (auto const &pair : ComponentFactoriesByName) {
        auto const &name = pair.first;
        auto const &factory = *pair.second;

        if (!factory.HasComponent(entity)) {
            continue;
        }

        if (!factory.Serialize(json[name], entity)) {
            // todo error message
            return false;
        }
    }

    return true;
}
} // namespace cfl
