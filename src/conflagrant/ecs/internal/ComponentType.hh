/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include <array>
#include <string>

#include "ClassType.hh"
#include "InternalEcsTypes.hh"

namespace cfl {
namespace ecs {
struct Component;

namespace internal {
/**
 * Convenience typedef for enumerating Component derived classes
 * from 0 and upwards.
 */
class ComponentType : public ClassType<component_id_t, Component> {
    struct Data {
        bool registered;
        size_t size;
        std::string name;
    };

    static std::array<Data, CFL_MAX_COMPONENT_TYPES> ComponentData;

    static component_id_t NumRegisteredTypes;

    ComponentType() = delete;

public:
    template<typename TComponent>
    static void RegisterComponentType(std::string const &name);

    static size_t GetComponentSize(component_id_t component_id);

    static std::string const &GetComponentName(component_id_t component_id);

    static component_id_t GetNumTypes();
};

template<typename TComponent>
void ComponentType::RegisterComponentType(std::string const &name) {
    component_id_t component_id = TypeId<TComponent>();
    Data &data = ComponentData[component_id];
    if (data.registered) {
        return;
    }

    data.registered = true;
    data.size = sizeof(TComponent);
    data.name = name;

    ++NumRegisteredTypes;
}

inline size_t ComponentType::GetComponentSize(component_id_t component_id) {
    return ComponentData[component_id].size;
}

inline std::string const &ComponentType::GetComponentName(component_id_t component_id) {
    return ComponentData[component_id].name;
}

inline component_id_t ComponentType::GetNumTypes() {
    return NumRegisteredTypes;
}
} // namespace internal
} // namespace ecs
} // namespace cfl
