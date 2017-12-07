#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/System.hh>
#include <conflagrant/factory_util.hh>

#include <entityx/System.h>
#include <unordered_map>


namespace cfl {
struct SystemFactory {
    virtual std::shared_ptr<cfl::System> Create(entityx::SystemManager &manager, Json::Value &json) const = 0;

    virtual bool HasSystem(entityx::SystemManager &manager) const = 0;

    virtual bool Serialize(Json::Value &json, entityx::SystemManager &manager) const = 0;

    virtual bool DrawWithImGui(entityx::SystemManager &manager, InputManager const &input) const = 0;

    virtual void Update(entityx::SystemManager &manager,
                        entityx::EntityManager &entities,
                        entityx::EventManager &events) const = 0;

    virtual string const GetName() const = 0;

    virtual bool IsSerializable() const = 0;

    virtual bool IsImGuiDrawable() const = 0;
};

template<typename TSystem>
class ConcreteSystemFactory : public SystemFactory {
    template<bool hasSerialize>
    typename std::enable_if<hasSerialize, bool>::type
    _Create(TSystem &system, Json::Value &json) const {
        return TSystem::Serialize(Deserializer(), json, system);
    }

    template<bool hasSerialize>
    typename std::enable_if<!hasSerialize, bool>::type
    _Create(TSystem &system, Json::Value &json) const {
        LOG_ERROR(ComponentFactory::Create) << "Called for component type without Serialize() function.";
        return false;
    }

    template<bool hasSerialize>
    typename std::enable_if<hasSerialize, bool>::type
    _Serialize(Json::Value &json, TSystem &system) const {
        return TSystem::Serialize(Serializer(), json, system);
    }

    template<bool hasSerialize>
    typename std::enable_if<!hasSerialize, bool>::type
    _Serialize(Json::Value &json, TSystem &system) const {
        LOG_ERROR(ComponentFactory::Serialize) << "Called for system type without Serialize() function.";
        return false;
    }

    template<bool hasDrawWithImGui>
    typename std::enable_if<hasDrawWithImGui, bool>::type
    _DrawWithImGui(TSystem &system, InputManager const &input) const {
        return TSystem::DrawWithImGui(system, input);
    }

    template<bool hasDrawWithImGui>
    typename std::enable_if<!hasDrawWithImGui, bool>::type
    _DrawWithImGui(TSystem &system, InputManager const &input) const {
        LOG_ERROR(SystemFactory::DrawWithImGui) << "Called for system type without DrawWithImGui() function.";
        return false;
    }

    static constexpr bool HasSerialize = has_Serialize<TSystem>::value;
    static constexpr bool HasDrawWithImGui = has_DrawWithImGui<TSystem>::value;

public:
    std::shared_ptr<System> Create(entityx::SystemManager &manager, Json::Value &json) const override {
        static_assert(std::is_base_of<System, TSystem>::value);
        static_assert(std::is_base_of<entityx::BaseSystem, TSystem>::value);

        std::shared_ptr<TSystem> system = manager.add<TSystem>();
        if (!_Create<HasSerialize>(*system, json)) {
            return nullptr;
        }

        return std::static_pointer_cast<System>(system);
    }

    bool HasSystem(entityx::SystemManager &manager) const override {
        return !!manager.system<TSystem>();
    }

    bool Serialize(Json::Value &json, entityx::SystemManager &manager) const override {
        assert(HasSystem(manager));
        return _Serialize<HasSerialize>(json, *manager.system<TSystem>());
    }

    bool DrawWithImGui(entityx::SystemManager &manager, InputManager const &input) const override {
        assert(HasSystem(manager));
        return _DrawWithImGui<HasDrawWithImGui>(*manager.system<TSystem>(), input);
    }

    void Update(entityx::SystemManager &manager,
                entityx::EntityManager &entities,
                entityx::EventManager &events) const override {
        assert(HasSystem(manager));
        manager.system<TSystem>()->update(entities, events, 0);
    }

    string const GetName() const override {
        return TSystem::SystemName;
    }

    bool IsSerializable() const override {
        return HasSerialize;
    }

    bool IsImGuiDrawable() const override {
        return HasDrawWithImGui;
    }
};

extern std::unordered_map<string, std::shared_ptr<SystemFactory>> SystemFactoriesByName;

#define REGISTER_SYSTEM(system_t) cfl::SystemFactoriesByName[system_t::SystemName] = \
std::static_pointer_cast<cfl::SystemFactory>(std::make_shared<cfl::ConcreteSystemFactory<system_t>>())
} // namespace cfl
