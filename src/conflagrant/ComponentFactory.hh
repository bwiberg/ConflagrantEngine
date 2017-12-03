#pragma once

#include <unordered_map>
#include <entityx/Entity.h>
#include <conflagrant/types.hh>
#include <conflagrant/serialization/Serialize.hh>

namespace cfl {
struct ComponentFactory {
    virtual bool Create(entityx::Entity &entity, Json::Value &json) const = 0;

    virtual bool HasComponent(entityx::Entity &entity) const = 0;

    virtual bool Serialize(Json::Value &json, entityx::Entity &entity) const = 0;

    virtual bool DrawWithImGui(entityx::Entity &entity, InputManager const &input) const = 0;
};

template<typename TComponent>
struct ConcreteComponentFactory : public ComponentFactory {
    bool Create(entityx::Entity &entity, Json::Value &json) const override {
        $
        entityx::ComponentHandle<TComponent> component = entity.assign<TComponent>();
        bool success = TComponent::template Serialize<Deserializer>(json, *component.get());
#ifdef NDEBUG
        return true;
#else
        return success;
#endif // NDEBUG
    }

    bool HasComponent(entityx::Entity &entity) const override {
        $
        return entity.has_component<TComponent>();
    }

    bool Serialize(Json::Value &json, entityx::Entity &entity) const override {
        $
        return TComponent::template Serialize<Serializer>(json, *entity.component<TComponent>());
    }

    bool DrawWithImGui(entityx::Entity &entity, InputManager const &input) const override {
        $
        assert(HasComponent(entity));
        return TComponent::DrawWithImGui(*entity.component<TComponent>(), input);
    }
};

extern std::unordered_map<string, std::shared_ptr<ComponentFactory>> ComponentFactoriesByName;

#define REGISTER_COMPONENT(component_t) cfl::ComponentFactoriesByName[component_t::GetName()] = \
std::static_pointer_cast<cfl::ComponentFactory>(std::make_shared<cfl::ConcreteComponentFactory<component_t>>())
} // namespace cfl
