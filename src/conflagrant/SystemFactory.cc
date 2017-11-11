#include "SystemFactory.hh"

namespace cfl {
std::unordered_map<string, std::shared_ptr<SystemFactory>> SystemFactoriesByName;
} // namespace cfl
