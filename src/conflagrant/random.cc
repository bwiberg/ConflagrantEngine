#include "random.hh"
#include <unordered_map>

namespace cfl {
namespace random {
std::unique_ptr<std::random_device> Device;
std::unique_ptr<std::unordered_map<
        string, std::shared_ptr<std::default_random_engine>
>> EnginesByTag;

bool Init() {
    if (Device) {
        return true;
    }

    Device = std::make_unique<std::random_device>();
    EnginesByTag = std::make_unique<std::unordered_map<
            string, std::shared_ptr<std::default_random_engine>
    >>();

    return true;
}

std::shared_ptr<std::default_random_engine> CreateEngine() {
    if (!Device || !EnginesByTag) {
        LOG_ERROR(cfl::random::CreateEngine) << "cfl::random::Init must be called once prior to this";
        return nullptr;
    }

    return std::make_shared<std::default_random_engine>(Device->operator()());
}

std::shared_ptr<std::default_random_engine> GetSharedEngine(string const &tag) {
    if (!Device || !EnginesByTag) {
        LOG_ERROR(cfl::random::CreateEngine) << "cfl::random::Init must be called once prior to this";
        return nullptr;
    }

    auto it = EnginesByTag->find(tag);
    if (it == EnginesByTag->end()) {
        return EnginesByTag->operator[](tag) = CreateEngine();
    }

    return it->second;
}
} // namespace random
} // namespace cfl
