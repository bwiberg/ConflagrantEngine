#pragma once

#include <unordered_map>
#include <entityx/System.h>

#include <conflagrant/types.hh>
#include <conflagrant/serialization/Serialize.hh>
#include <conflagrant/System.hh>

namespace cfl {
struct SystemFactory {
    virtual std::shared_ptr<cfl::System> Create(entityx::SystemManager &manager, Json::Value &json) const = 0;

    virtual bool HasSystem(entityx::SystemManager &manager) const = 0;

    virtual bool Serialize(Json::Value &json, entityx::SystemManager &manager) const = 0;
};

template<typename TSystem>
struct ConcreteSystemFactory : public SystemFactory {
    std::shared_ptr<System> Create(entityx::SystemManager &manager, Json::Value &json) const override {
        static_assert(std::is_base_of<System, TSystem>::value);

        std::shared_ptr<TSystem> system = manager.add<TSystem>();
        if (!TSystem::template Serialize<Deserializer>(json, *system)) {
            return nullptr;
        }

        return std::static_pointer_cast<System>(system);
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
std::static_pointer_cast<cfl::SystemFactory>(std::make_shared<cfl::ConcreteSystemFactory<system_t>>())
} // namespace cfl