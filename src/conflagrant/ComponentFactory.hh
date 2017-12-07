#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/factory_util.hh>

#include <entityx/Entity.h>
#include <unordered_map>

namespace cfl {
struct ComponentFactory {
    virtual bool Create(entityx::Entity &entity, Json::Value &json) const = 0;

    virtual bool HasComponent(entityx::Entity &entity) const = 0;

    virtual bool Serialize(Json::Value &json, entityx::Entity &entity) const = 0;

    virtual bool DrawWithImGui(entityx::Entity &entity, InputManager const &input) const = 0;

    virtual string const GetName() const = 0;

    virtual bool IsSerializable() const = 0;

    virtual bool IsImGuiDrawable() const = 0;
};

template<typename TComponent>
class ConcreteComponentFactory : public ComponentFactory {
    template<bool hasSerialize>
    typename std::enable_if<hasSerialize, bool>::type
    _Create(Json::Value &json, entityx::Entity &entity) const {
        return TComponent::Serialize(Deserializer(), json, *entity.component<TComponent>());
    }

    template<bool hasSerialize>
    typename std::enable_if<!hasSerialize, bool>::type
    _Create(Json::Value &json, entityx::Entity &entity) const {
        LOG_ERROR(ComponentFactory::Create) << "Called for component type without Serialize() function.";
        return false;
    }

    template<bool hasSerialize>
    typename std::enable_if<hasSerialize, bool>::type
    _Serialize(Json::Value &json, entityx::Entity &entity) const {
        return TComponent::Serialize(Serializer(), json, *entity.component<TComponent>());
    }

    template<bool hasSerialize>
    typename std::enable_if<!hasSerialize, bool>::type
    _Serialize(Json::Value &json, entityx::Entity &entity) const {
        LOG_ERROR(ComponentFactory::Serialize) << "Called for component type without Serialize() function.";
        return false;
    }

    template<bool hasDrawWithImGui>
    typename std::enable_if<hasDrawWithImGui, bool>::type
    _DrawWithImGui(entityx::Entity &entity, InputManager const &input) const {
        return TComponent::DrawWithImGui(*entity.component<TComponent>(), input);
    }

    template<bool hasDrawWithImGui>
    typename std::enable_if<!hasDrawWithImGui, bool>::type
    _DrawWithImGui(entityx::Entity &entity, InputManager const &input) const {
        LOG_ERROR(ComponentFactory::DrawWithImGui) << "Called for component type without DrawWithImGui() function.";
        return false;
    }

    static constexpr bool HasSerialize = has_Serialize<TComponent>::value;
    static constexpr bool HasDrawWithImGui = has_DrawWithImGui<TComponent>::value;

public:
    bool Create(entityx::Entity &entity, Json::Value &json) const override {
        entity.assign<TComponent>();
        bool success = _Create<HasSerialize>(json, entity);
#ifdef NDEBUG
        return true;
#else
        return success;
#endif // NDEBUG
    }

    bool HasComponent(entityx::Entity &entity) const override {
        return entity.has_component<TComponent>();
    }

    bool Serialize(Json::Value &json, entityx::Entity &entity) const override {
        assert(HasComponent(entity));
        return _Serialize<HasSerialize>(json, entity);
    }

    bool DrawWithImGui(entityx::Entity &entity, InputManager const &input) const override {
        assert(HasComponent(entity));
        return _DrawWithImGui<HasDrawWithImGui>(entity, input);
    }

    string const GetName() const override {
        return TComponent::ComponentName;
    }

    bool IsSerializable() const override {
        return HasSerialize;
    }

    bool IsImGuiDrawable() const override {
        return HasDrawWithImGui;
    }
};

extern std::unordered_map<string, std::shared_ptr<ComponentFactory>> ComponentFactoriesByName;

#define REGISTER_COMPONENT(component_t) cfl::ComponentFactoriesByName[component_t::ComponentName] = \
std::static_pointer_cast<cfl::ComponentFactory>(std::make_shared<cfl::ConcreteComponentFactory<component_t>>())
} // namespace cfl
