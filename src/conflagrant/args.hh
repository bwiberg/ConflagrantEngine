#pragma once

#include <conflagrant/types.hh>
#include <unordered_map>

namespace cfl {
namespace args {
class Arguments {
    std::unordered_map<string, string> arguments;

public:
    Arguments(int argc, char *argv[]);
};
} // namespace args
} // namespace cfl
