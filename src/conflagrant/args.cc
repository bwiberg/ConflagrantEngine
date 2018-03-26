#include "args.hh"

#include <vector>

namespace cfl {
namespace args {
Arguments::Arguments(int argc, char **argv) {
    std::vector<std::string> a(argv + 1, argv + argc);

    auto it = a.cbegin();

    while (it != a.cend()) {
        ++it;
    }
}
} // namespace args
} // namespace cfl
