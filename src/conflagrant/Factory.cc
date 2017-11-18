#include "SystemFactory.hh"
#include "ComponentFactory.hh"

namespace cfl {
std::unordered_map<string, std::shared_ptr<SystemFactory>> SystemFactoriesByName;
std::unordered_map<string, std::shared_ptr<ComponentFactory>> ComponentFactoriesByName;
} // namespace cfl
