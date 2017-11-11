#pragma once

#include <unordered_map>
#include <entityx/System.h>
#include <conflagrant/types.hh>
#include <conflagrant/serialization/Serialize.hh>

namespace cfl {
struct SystemFactory {
    virtual bool Create(entityx::SystemManager &manager, Json::Value &json) const = 0;

    virtual bool HasSystem(entityx::SystemManager &manager) const = 0;

    virtual bool Serialize(Json::Value &json, entityx::SystemManager &manager) const = 0;
};

template<typename TSystem>
struct ConcreteSystemFactory : public SystemFactory {
    bool Create(entityx::SystemManager &manager, Json::Value &json) const override {
        std::shared_ptr<TSystem> system = manager.add<TSystem>();
        return TSystem::template Serialize<Deserializer>(json, *system);
    }

    bool HasSystem(entityx::SystemManager &manager) const override {
        return !!manager.system<TSystem>();
    }

    bool Serialize(Json::Value &json, entityx::SystemManager &manager) const override {
        assert(HasSystem(manager));
        std::shared_ptr<TSystem> system = manager.system<TSystem>();
        return TSystem::template Serialize<Serializer>(json, *system);
    }
};

extern std::unordered_map<string, std::shared_ptr<SystemFactory>> SystemFactoriesByName;

#define REGISTER_SYSTEM(system_t) cfl::SystemFactoriesByName[system_t::GetName()] = \
std::make_shared<cfl::ConcreteSystemFactory<system_t>>()

#define CLEAR_REGISTERED_SYSTEMS() cfl::SystemFactoriesByName.clear()
} // namespace cfl
