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

    virtual bool DrawWithImGui(entityx::SystemManager &manager, InputManager const &input) const = 0;

    virtual void Update(entityx::SystemManager &manager,
                        entityx::EntityManager &entities,
                        entityx::EventManager &events) const = 0;

    virtual string GetName() const = 0;
};

template<typename TSystem>
struct ConcreteSystemFactory : public SystemFactory {
    std::shared_ptr<System> Create(entityx::SystemManager &manager, Json::Value &json) const override {
        static_assert(std::is_base_of<System, TSystem>::value);
        static_assert(std::is_base_of<entityx::BaseSystem, TSystem>::value);

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

    bool DrawWithImGui(entityx::SystemManager &manager, InputManager const &input) const override {
        assert(HasSystem(manager));
        return TSystem::DrawWithImGui(*manager.system<TSystem>(), input);
    }

    void Update(entityx::SystemManager &manager,
                entityx::EntityManager &entities,
                entityx::EventManager &events) const override {
        assert(HasSystem(manager));
        manager.system<TSystem>()->update(entities, events, 0);
    }

    string GetName() const override {
        return TSystem::GetName();
    }
};

extern std::unordered_map<string, std::shared_ptr<SystemFactory>> SystemFactoriesByName;

#define REGISTER_SYSTEM(system_t) cfl::SystemFactoriesByName[system_t::GetName()] = \
std::static_pointer_cast<cfl::SystemFactory>(std::make_shared<cfl::ConcreteSystemFactory<system_t>>())
} // namespace cfl
