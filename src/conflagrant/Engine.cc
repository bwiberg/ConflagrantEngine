#include "Engine.hh"
#include "Time.hh"

#include <conflagrant/ComponentFactory.hh>

#include <fstream>

namespace cfl {
Engine::Engine(std::shared_ptr<Window> window)
        : window(window) {
    assert(window != nullptr);
    input = std::make_shared<InputManager>(window);
}

Engine::~Engine() {
    entities->reset();
}

bool Engine::LoadScene(string const &pathToJson) {
#define RETURN_ERROR(message) LOG_ERROR(cfl::Engine::LoadScene) << (message); return false;

    Path path(pathToJson);
    if (!path.is_file()) {
        RETURN_ERROR("Supplied pathToJson is not a file.");
    }

    string const &extension = path.extension();
    if (extension != "cfl" && extension != "json") {
        RETURN_ERROR("Supplied file is not a .cfl or .json file.");
    }

    std::ifstream file(pathToJson);
    if (!file.is_open()) {
        LOG_ERROR(cfl::Engine::LoadScene) << "Could not open '" << pathToJson << "' for reading. Exiting.";
        return false;
    }

    Json::Value json;
    file >> json;

    Path jsonDirectory = path.parent_path();
    Path defaultAssetsPath = jsonDirectory / Path("assets");
    assets::AssetManager::AddAssetsPath(defaultAssetsPath);

    bool success = LoadScene([this, &json, &jsonDirectory](std::shared_ptr<entityx::EntityManager> entities,
                                                           std::shared_ptr<entityx::SystemManager> systems) {
        if (!json.isObject()) {
            RETURN_ERROR("Json is not an object.");
        }

        if (json.isMember("assetPaths")) {
            Json::Value &jsonAssetPaths = json["assetPaths"];
            if (jsonAssetPaths.isArray()) {
                for (auto const &jsonAssetPath : jsonAssetPaths) {
                    if (!jsonAssetPath.isString()) {
                        LOG_INFO(cfl::Engine::LoadScene)
                                << "Encountered element in 'assetPaths' that is not a string.";
                        continue;
                    }

                    Path relativeAssetsPath(jsonAssetPath.asString());
                    Path assetsPath = jsonDirectory / relativeAssetsPath;
                    if (!assetsPath.exists() || !assetsPath.is_directory()) {
                        LOG_INFO(cfl::Engine::LoadScene) << "Encountered relative assetPath '" << relativeAssetsPath
                                                         << "' that expands to '" << assetsPath
                                                         << "' which isn't a valid directory.";
                        continue;
                    }

                    assets::AssetManager::AddAssetsPath(assetsPath);
                }
            }
        }

        if (!json.isMember("systems")) {
            RETURN_ERROR("Json does not contain \"systems\".");
        }

        if (!json.isMember("entities")) {
            RETURN_ERROR("Json does not contain \"entities\"");
        }

        if (!CreateSystems(json["systems"])) {
            RETURN_ERROR("Failed to create systems from json.");
        }

        if (!CreateEntities(json["entities"])) {
            RETURN_ERROR("Failed to create entities from json.");
        }

        return true;
    });

    if (success) {
        currentScenePath = std::make_unique<cfl::Path>(std::move(path));
    }

    return success;
#undef RETURN_ERROR
}

bool Engine::LoadScene(Json::Value &json) {
#define RETURN_ERROR(message) LOG_ERROR(cfl::Engine::LoadScene) << (message); return false;
    return LoadScene([this, &json](std::shared_ptr<entityx::EntityManager> entities,
                                   std::shared_ptr<entityx::SystemManager> systems) {
        if (!json.isObject()) {
            RETURN_ERROR("Json is not an object.");
        }

        if (!json.isMember("systems")) {
            RETURN_ERROR("Json does not contain \"systems\".");
        }

        if (!json.isMember("entities")) {
            RETURN_ERROR("Json does not contain \"entities\"");
        }

        if (!CreateSystems(json["systems"])) {
            RETURN_ERROR("Failed to create systems from json.");
        }

        if (!CreateEntities(json["entities"])) {
            RETURN_ERROR("Failed to create entities from json.");
        }

        return true;
    });
#undef RETURN_ERROR
}

bool Engine::LoadScene(std::function<bool(std::shared_ptr<entityx::EntityManager>,
                                          std::shared_ptr<entityx::SystemManager>)> loadSceneFunction) {
    UnloadScene();

    events = std::make_shared<entityx::EventManager>();
    entities = std::make_shared<entityx::EntityManager>(*events);
    systems = std::make_shared<entityx::SystemManager>(*entities, *events);

    if (!loadSceneFunction(entities, systems)) {
        LOG_ERROR(cfl::Engine::LoadScene) << "Provided loadSceneFunction returns false.";
        return false;
    }

    systems->configure();
    return true;
}

bool Engine::CreateSystems(Json::Value &json) {
    if (!json.isArray()) {
        LOG_ERROR(cfl::Engine::CreateSystems) << "Json is not an array.";
        return false;
    }

    Json::ArrayIndex const NumSystems = json.size();
    for (Json::ArrayIndex i = 0; i < NumSystems; ++i) {
        Json::Value &jsonSystem = json[i];

        std::string systemName = jsonSystem["name"].asString();

        auto it = SystemFactoriesByName.find(systemName);
        if (it == SystemFactoriesByName.end()) {
            LOG_ERROR(cfl::Engine::CreateSystems) << "System with name '" << systemName << "' does not have a factory."
                                                  << " Have you forgot to register the system?";
            return false;
        }

        std::shared_ptr<SystemFactory> factory = it->second;
        orderedSystemFactories.push_back(factory);

        auto system = factory->Create(*systems, jsonSystem);
        if (!system) {
            LOG_ERROR(cfl::Engine::CreateSystems) << "Failed to create system with name '" << systemName << "'.";
            return false;
        }

        assert(input != nullptr);
        assert(window != nullptr);

        system->input = input;
        system->window = window;
        system->engine = this;

        systemVector.push_back(system);
    }

    return true;
}

bool Engine::CreateEntities(Json::Value &jsonEntities) {
    if (!jsonEntities.isArray()) {
        LOG_ERROR(cfl::Engine::CreateEntities) << "Json is not an array.";
        return false;
    }

    Json::ArrayIndex const NumEntities = jsonEntities.size();
    for (Json::ArrayIndex i = 0; i < NumEntities; ++i) {
        if (!CreateEntity(jsonEntities[i])) {
            LOG_ERROR(cfl::Engine::CreateEntities) << "Failed to create entity from Json.";
            return false;
        }
    }

    return true;
}

bool Engine::CreateEntity(Json::Value &jsonEntity) {
    entityx::Entity entity = entities->create();
#define RETURN_ERROR() { entity.destroy(); return false; }

    if (!jsonEntity.isObject()) {
        LOG_ERROR(cfl::Engine::CreateEntity) << "Json is not an object.";
        RETURN_ERROR();
    }

    // loop through each json object member and try to create a component matching the member's name
    Json::Value::Members const &componentNames = jsonEntity.getMemberNames();
    for (auto const &componentName : componentNames) {
        auto it = ComponentFactoriesByName.find(componentName);
        if (it == ComponentFactoriesByName.end()) {
            LOG_ERROR(cfl::Engine::CreateEntity) << "Component with name '" << componentName
                                                 << "' does not have a factory. "
                                                 << "Have you forgot to register the component?";
            RETURN_ERROR();
        }

        auto &factory = *it->second;
        if (!factory.IsSerializable()) {
            LOG_ERROR(cfl::Engine::CreateEntity) << "Component with name '" << componentName
                                                 << "' is not serializable (i.e. do not specify it in your scene file)";
        }

        if (!factory.Create(entity, jsonEntity[componentName])) {
            LOG_ERROR(cfl::Engine::CreateEntity) << "Failed to create component with name '" << componentName << "'.";
            RETURN_ERROR();
        }
    }

    return true;
#undef RETURN_ERROR
}

int Engine::Run(bool singleTimestep) {
    shouldStop = singleTimestep;
    window->SetTime(0);
    Time::previousFrameTime = Time::currentFrameTime = 0;
    do {
        dollar::clear();
        Time::RecordCurrentFrameTime(window->GetTime());

        if (input) {
            input->ProcessInput();
            if (input->GetKey(Key::ESCAPE)) {
                break;
            }
        }

        if (window) window->BeginFrame();
        std::stringstream ss;
        for (auto &factory : orderedSystemFactories) {
            factory->Update(*systems, *entities, *events);
            ss << factory->GetName() << ", ";
        }
        Log::DrawImGuiWindow();

        ImGui::Begin("Performance");
        if (ImGui::Button("Save chrome://tracing file")) {
            std::ofstream file("/home/bwiberg/confl.json");
            dollar::chrome(file);
        }
        ImGui::End();

        if (window) window->FinishFrame();
        dollar::clear();

        if (currentScenePath && input->AllKeysHeldAtLeastOneKeyDown({Key::LEFT_CONTROL, Key::S})) {
            // save scene to loaded json file
            Json::Value savedScene;
            if (!SaveScene(savedScene)) {
                LOG_ERROR(cfl::Engine::Run) << "Failed to export current scene to Json::Value";
                continue;
            }

            std::ofstream file(currentScenePath->str());
            if (!file.is_open()) {
                LOG_ERROR(cfl::Engine::Run) << "Could not open '" << currentScenePath->str() << "' for writing.";
                continue;
            }

            file << savedScene;

            file.close();
            if (file.bad()) {
                LOG_ERROR(cfl::Engine::Run) << "Failed to write to '" << currentScenePath->str() << "'.";
                continue;
            }

            LOG_INFO(cfl::Engine::Run) << "Successfully saved scene to '" << currentScenePath->str() << "'.";
        }

    } while (!shouldStop);

    shouldStop = false;
    return 0;
}

// save scene

bool Engine::SaveScene(Json::Value &json) {
    if (!SaveSystems(json["systems"])) {
        LOG_ERROR(cfl::Engine::SaveScene) << "Failed to save systems to Json";
        return false;
    }

    if (!SaveEntities(json["entities"])) {
        LOG_ERROR(cfl::Engine::SaveScene) << "Failed to save entities to Json";
        return false;
    }

    return true;
}

bool Engine::UnloadScene() {
    for (auto &system : systemVector) {
        system->window = nullptr;
        system->input = nullptr;
        system->engine = nullptr;
    }
    systemVector.clear();
    orderedSystemFactories.clear();

    events = std::make_shared<entityx::EventManager>();
    entities = std::make_shared<entityx::EntityManager>(*events);
    systems = std::make_shared<entityx::SystemManager>(*entities, *events);

    return false;
}

bool Engine::SaveSystems(Json::Value &json) {
    for (auto const &factory : orderedSystemFactories) {
        if (!factory->HasSystem(*systems)) {
            continue;
        }

        Json::Value jsonSystem;
        if (!factory->Serialize(jsonSystem, *systems)) {
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

        if (!factory.IsSerializable() || !factory.HasComponent(entity)) {
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
