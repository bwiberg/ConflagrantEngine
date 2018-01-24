#include <conflagrant/SystemFactory.hh>
#include <conflagrant/ComponentFactory.hh>
#include <conflagrant/procgen/MeshGenerator.hh>

namespace cfl {
std::unordered_map<string, std::shared_ptr<SystemFactory>> SystemFactoriesByName;
std::unordered_map<string, std::shared_ptr<ComponentFactory>> ComponentFactoriesByName;

namespace procgen {
std::unordered_map<string, std::shared_ptr<MeshGeneratorFactory>> MeshGeneratorFactoriesByName;
}
} // namespace cfl
