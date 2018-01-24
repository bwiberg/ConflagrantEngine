#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <random>

namespace cfl {
namespace random {
bool Init();

std::shared_ptr<std::default_random_engine> CreateEngine();

std::shared_ptr<std::default_random_engine> GetSharedEngine(string const &tag);
} // namespace random
} // namespace cfl
